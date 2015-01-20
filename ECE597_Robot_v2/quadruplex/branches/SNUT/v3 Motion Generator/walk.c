moveMotor(0,564);
moveMotor(1,514);
moveMotor(2,433);
moveMotor(3,460);
moveMotor(4,639);
moveMotor(5,515);
moveMotor(6,564);
moveMotor(7,385);
moveMotor(8,509);
moveMotor(9,460);
moveMotor(10,680);
moveMotor(11,591);
wait_ms(1000);

moveMotor(0,645);
moveMotor(1,456);
moveMotor(2,568);
wait_ms(1000);

moveMotor(1,311);
moveMotor(2,629);
wait_ms(1000);

moveMotor(1,244);
moveMotor(2,568);
wait_ms(1000);

moveMotor(0,564);
moveMotor(1,344);
moveMotor(2,433);
wait_ms(1000);

moveMotor(9,379);
moveMotor(10,780);
moveMotor(11,456);
wait_ms(1000);

moveMotor(10,713);
moveMotor(11,395);
wait_ms(1000);

moveMotor(10,568);
moveMotor(11,456);
wait_ms(1000);

moveMotor(9,460);
moveMotor(10,510);
moveMotor(11,591);
wait_ms(1000);

moveMotor(1,351);
moveMotor(2,491);
moveMotor(4,584);
moveMotor(5,533);
moveMotor(7,351);
moveMotor(8,491);
moveMotor(10,584);
moveMotor(11,533);
wait_ms(1000);

moveMotor(1,385);
moveMotor(2,509);
moveMotor(4,510);
moveMotor(5,591);
moveMotor(7,344);
moveMotor(8,433);
moveMotor(10,639);
moveMotor(11,515);
wait_ms(1000);

moveMotor(3,379);
moveMotor(4,568);
moveMotor(5,456);
wait_ms(1000);

moveMotor(4,713);
moveMotor(5,395);
wait_ms(1000);

moveMotor(4,780);
moveMotor(5,456);
wait_ms(1000);

moveMotor(3,460);
moveMotor(4,680);
moveMotor(5,591);
wait_ms(1000);

moveMotor(6,645);
moveMotor(7,244);
moveMotor(8,568);
wait_ms(1000);

moveMotor(7,311);
moveMotor(8,629);
wait_ms(1000);

moveMotor(7,456);
moveMotor(8,568);
wait_ms(1000);

moveMotor(6,564);
moveMotor(7,514);
moveMotor(8,433);
wait_ms(1000);

moveMotor(1,440);
moveMotor(2,491);
moveMotor(4,673);
moveMotor(5,533);
moveMotor(7,440);
moveMotor(8,491);
moveMotor(10,673);
moveMotor(11,533);
wait_ms(1000);

moveMotor(1,514);
moveMotor(2,433);
moveMotor(4,639);
moveMotor(5,515);
moveMotor(7,385);
moveMotor(8,509);
moveMotor(10,680);
moveMotor(11,591);
wait_ms(1000);

static char walk[84][4]={
{100,0,0x34,0x2},
{0,1,0x2,0x2},
{0,2,0xB1,0x1},
{0,3,0xCC,0x1},
{0,4,0x7F,0x2},
{0,5,0x3,0x2},
{0,6,0x34,0x2},
{0,7,0x81,0x1},
{0,8,0xFD,0x1},
{0,9,0xCC,0x1},
{0,10,0xA8,0x2},
{0,11,0x4F,0x2},
{100,0,0x85,0x2},
{0,1,0xC8,0x1},
{0,2,0x38,0x2},
{100,1,0x37,0x1},
{0,2,0x75,0x2},
{100,1,0xF4,0x0},
{0,2,0x38,0x2},
{100,0,0x34,0x2},
{0,1,0x58,0x1},
{0,2,0xB1,0x1},
{100,9,0x7B,0x1},
{0,10,0xC,0x3},
{0,11,0xC8,0x1},
{100,10,0xC9,0x2},
{0,11,0x8B,0x1},
{100,10,0x38,0x2},
{0,11,0xC8,0x1},
{100,9,0xCC,0x1},
{0,10,0xFE,0x1},
{0,11,0x4F,0x2},
{100,1,0x5F,0x1},
{0,2,0xEB,0x1},
{0,4,0x48,0x2},
{0,5,0x15,0x2},
{0,7,0x5F,0x1},
{0,8,0xEB,0x1},
{0,10,0x48,0x2},
{0,11,0x15,0x2},
{100,1,0x81,0x1},
{0,2,0xFD,0x1},
{0,4,0xFE,0x1},
{0,5,0x4F,0x2},
{0,7,0x58,0x1},
{0,8,0xB1,0x1},
{0,10,0x7F,0x2},
{0,11,0x3,0x2},
{100,3,0x7B,0x1},
{0,4,0x38,0x2},
{0,5,0xC8,0x1},
{100,4,0xC9,0x2},
{0,5,0x8B,0x1},
{100,4,0xC,0x3},
{0,5,0xC8,0x1},
{100,3,0xCC,0x1},
{0,4,0xA8,0x2},
{0,5,0x4F,0x2},
{100,6,0x85,0x2},
{0,7,0xF4,0x0},
{0,8,0x38,0x2},
{100,7,0x37,0x1},
{0,8,0x75,0x2},
{100,7,0xC8,0x1},
{0,8,0x38,0x2},
{100,6,0x34,0x2},
{0,7,0x2,0x2},
{0,8,0xB1,0x1},
{100,1,0xB8,0x1},
{0,2,0xEB,0x1},
{0,4,0xA1,0x2},
{0,5,0x15,0x2},
{0,7,0xB8,0x1},
{0,8,0xEB,0x1},
{0,10,0xA1,0x2},
{0,11,0x15,0x2},
{100,1,0x2,0x2},
{0,2,0xB1,0x1},
{0,4,0x7F,0x2},
{0,5,0x3,0x2},
{0,7,0x81,0x1},
{0,8,0xFD,0x1},
{0,10,0xA8,0x2},
{0,11,0x4F,0x2}};
