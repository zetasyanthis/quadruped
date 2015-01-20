#include <stdlib.h>
#include <glib.h>
#include <string.h>
#include "rx28.h"
#include "rs485.h"

static gint _compute_checksum(const struct packet *p, guint8 *checksum);
static gboolean _valid_checksum(const struct packet *p);
static const gchar *_instruction_to_string(guint8 instruction);
static gchar * _error_to_string(guint8 error);

void
rx28_print_packet(const struct packet *p)
{

   gchar *errstr;

   g_print("id:\t%02X\n", p->id);
   g_print("len:\t%02X\n", p->length);
   g_print("inst:\t%02X (%s)\n", p->instruction,
         _instruction_to_string(p->instruction));
   // For status packets only.  Maybe add a flag indicating whether this is a
   // status packet and conditionally print either this or the above.
   errstr = _error_to_string(p->instruction);
   g_print("error:\t%02X (%s)\n", p->instruction, errstr);
   free(errstr);
   for (gint i = 0; i < (p->length-2) && p->param; i++)
      g_print("%02X ", p->param[i]);
   g_print("chk:\t%02X (%s)\n", p->checksum, _valid_checksum(p) ? "Valid" : "Invalid");

}

/* NB: caller must free returned string. */
static gchar *
_error_to_string(guint8 error)
{
   GString *errstr;
   errstr = g_string_new("");

   if (error & kRX28ErrorInstruction)
      g_string_append(errstr, "Instruction Error ");
   if (error & kRX28ErrorOverload)
      g_string_append(errstr, "Overload Error ");
   if (error & kRX28ErrorChecksum)
      g_string_append(errstr, "Checksum Error ");
   if (error & kRX28ErrorRange)
      g_string_append(errstr, "Range Error ");
   if (error & kRX28ErrorOverheating)
      g_string_append(errstr, "Overheating Error ");
   if (error & kRX28ErrorAngleLimit)
      g_string_append(errstr, "Angle Limit Error ");
   if (error & kRX28ErrorInputVoltage)
      g_string_append(errstr, "Input Voltage Error ");

   return g_string_free(errstr, FALSE);
}

static const gchar *
_instruction_to_string(guint8 instruction)
{
   switch (instruction) {
      case kRX28InstrReadData:  return "Read Data";
      case kRX28InstrWriteData: return "Write Data";
      case kRX28InstrRegWrite:  return "Register Write";
      case kRX28InstrSyncWrite: return "Synchronous Write";
      case kRX28InstrAction:    return "Action";
      case kRX28InstrPing:      return "Ping!";
      case kRX28InstrReset:     return "Reset";
      default:                  return "Unknown instruction (error?)";
   }
};

static gint
_compute_checksum(const struct packet *p, guint8 *checksum)
{
   if (!checksum) return -1;

   guint16 tmp;
   tmp = p->id + p->length + p->instruction;

   if ((p->length - 2) != 0 && !p->param) return -1;
   for (int i = 0; i < (p->length - 2); i++)
      tmp += p->param[i];

   *checksum = ~(guint8)(tmp & 0xFF);

   return 0;
}

static gboolean
_valid_checksum(const struct packet *p)
{
   guint8 checksum;

   if (_compute_checksum(p, &checksum) < 0)
      return FALSE;

   if (checksum == p->checksum)
      return TRUE;

   return FALSE;
}

int
rx28_send_packet(struct packet *p, struct packet *status)
{
   if (!p) return -1;

   guint8 packet_size = 2 + 1 + 1 + 1 + p->length-2 + 1;
   guint8 i = 0;
   guint8 buf[packet_size];

   _compute_checksum(p, &p->checksum);

   buf[i++] = 0xFF;
   buf[i++] = 0xFF;
   buf[i++] = p->id;
   buf[i++] = p->length;
   buf[i++] = p->instruction;
   for (int j = 0; (p->param) && (j < p->length-2); j++)
      buf[i++] = p->param[j];
   buf[i++] = p->checksum;

   return rs485_send(buf, packet_size, NULL);
}

/*
 * ping the rx28 identified by id, returns status packet if status != null
 * returns TRUE if the rx28 is there, FALSE otherwise
 */
gboolean
rx28_ping(guint8 id, struct packet *status)
{

   struct packet ping = {
      .id = id,
      .length = 2,
      .instruction = kRX28InstrPing,
      .param = NULL,
   };
   rx28_send_packet(&ping, NULL);

   struct packet stat;
   if (rx28_receive_status_packet(&stat) == 0) {
      //rx28_print_packet(&stat);
      return TRUE;
   }
   return FALSE;
}

gint
rx28_reg_write_byte(guint8 id, guint8 addr, guint8 byte)
{
   return rx28_reg_write(id, addr, &byte, 1);
}

gint
rx28_reg_write_word(guint8 id, guint8 addr, guint16 word)
{
   guint8 bytes[2] = { word & 0xFF, word >> 010 };
   return rx28_reg_write(id, addr, bytes, 2);
}

gint
rx28_reg_write(guint8 id, guint8 addr, const guint8 *buf, guint8 len)
{
   guint8 param[len + 1];
   param[0] = addr;
   memcpy(param+1, buf, len);

   struct packet reg_write = {
      .id = id,
      .instruction = kRX28InstrRegWrite,
      .length = 2 + 1 + len,
      .param = param
   };

   rx28_send_packet(&reg_write, NULL);

   struct packet status;
   if (rx28_receive_status_packet(&status) != 0) {
      g_warning("No status packet received...");
      return -1;
   }

   return 0;
}




/*
 * perform a synchronous write
 *    addr: the starting address for the write
 *    sync_write: struct containing the id of a motor, and the data to be written
 *    n_ids: the number of motors to write to
 *    len_per_id: the number of bytes to be written to each motor
 */
gint
rx28_sync_write(guint8 addr, struct sync_write *data, guint8 n_ids, guint8 len_per_id)
{
   guint8 len = (len_per_id + 1) * n_ids + 2;
   guint8 param[len];
   struct packet action = {
      .id = 0xFE,
      .instruction = kRX28InstrSyncWrite,
      .param = param,
      .length = len + 2,
   };

   gint j = 0;
   param[j++] = addr;
   param[j++] = len_per_id;
   for (gint i = 0; i < n_ids; i++) {
      param[j++] = data[i].id;
      for (gint k = 0; k < len_per_id; k++) {
         param[j++] = data[i].data[k]; // that's clear...
      }
   }

   rx28_send_packet(&action, NULL);

   return 0;
}

      




gint
rx28_execute_action(void)
{
   struct packet action = {
      .id = 0xFE,
      .instruction = kRX28InstrAction,
      .param = NULL,
      .length = 2
   };

   rx28_send_packet(&action, NULL);
   return 0;
}

inline gint
rx28_write_data(guint8 id, guint8 addr, const guint8 *buf, guint8 len)
{
   guint8 param[len + 1];
   param[0] = addr;
   memcpy(param+1, buf, len);

   struct packet write_data = {
      .id = id,
      .instruction = kRX28InstrWriteData,
      .length = 2 + 1 + len,
      .param = param
   };
   rx28_send_packet(&write_data, NULL);

   struct packet status;
   if (rx28_receive_status_packet(&status)) goto error;
   return 0;

error:
   g_warning("No status packet received...");
   return -1;
}

gint
rx28_write_data_byte(guint8 id, guint8 addr, guint8 byte)
{
   return rx28_write_data(id, addr, &byte, 1);
}

gint
rx28_write_data_word(guint8 id, guint8 addr, guint16 word)
{
   guint8 bytes[2] = { word & 0xFF, word >> 010 };
   return rx28_write_data(id, addr, bytes, 2);
}

gint
rx28_read_data(guint8 id, guint8 addr, guint8 *buf, guint8 len)
{
   guint8 param[2] = { addr, len };
   struct packet read_data = {
      .id = id,
      .instruction = kRX28InstrReadData,
      .param = param,
      .length = 4,
   };
   rx28_send_packet(&read_data, NULL);

   struct packet status;
   if (rx28_receive_status_packet(&status)) goto error;

   // TODO: skip this if buf is null
   if (status.param) {
         memcpy(buf, status.param, MIN(len, status.length-2));
         free(status.param);
   }

   return 0;

error:
   g_warning("invalid status packet!");
   free(status.param);
   return -1;
}

gint
rx28_read_data_byte(guint8 id, guint8 addr, guint8 *byte)
{
   if (!byte) return -1;
   return rx28_read_data(id, addr, byte, 1);
}

gint
rx28_read_data_word(guint8 id, guint8 addr, guint16 *word)
{
   if (!word) return -1;
   guint8 bytes[2];
   gint rv;

   // TODO error check
   rv = rx28_read_data(id, addr, bytes, 2);
   *word = (bytes[0] | (bytes[1] << 010));
   return rv;
}



gint
rx28_receive_status_packet(struct packet *status)
{
   gint c;
   gint state, next_state;
   guint8 param_length = 0;
   struct packet tmpstatus;
   enum {
      kHeader1,
      kHeader2,
      kId,
      kLength,
      kInstruction,
      kParam,
      kChecksum,
   };

   tmpstatus.param = NULL;
   state = kHeader1;
   // Flying Spaghetti Monster!
   while ((c = rs485_getc()) != -1) {
      switch (state) {
      case kHeader1:
         if (c == 0xFF) next_state = kHeader2;
         else           next_state = kHeader1;
         break;
      case kHeader2:
         if (c == 0xFF) next_state = kId;
         else           next_state = kHeader1;
         break;
      case kId:
         tmpstatus.id = c;
         next_state = kLength;
         break;
      case kLength:
         tmpstatus.length = c;
         next_state = kInstruction;
         break;
      case kInstruction:
         tmpstatus.instruction = c;
         if (tmpstatus.length == 2) next_state = kChecksum;
         else                       next_state = kParam;
         break;
      case kParam:
         if (tmpstatus.param == NULL) {
            tmpstatus.param = malloc(tmpstatus.length-2);
            if (tmpstatus.param == NULL) return -1;
         }
         tmpstatus.param[param_length++] = c;
         if (param_length == (tmpstatus.length-2)) next_state = kChecksum;
         else                                      next_state = kParam;
         break;
      case kChecksum:
         tmpstatus.checksum = c;
         goto out;
      default:
         return -1;
      }
      state = next_state;
   }

out:
   if (status)
     memcpy(status, &tmpstatus, sizeof(struct packet));
   else
      if (tmpstatus.param) free(tmpstatus.param);

   if (_valid_checksum(&tmpstatus))
         return 0;

   return -1;
}
