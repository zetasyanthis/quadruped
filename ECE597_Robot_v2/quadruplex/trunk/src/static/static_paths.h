
const guint8 stand[][4]={
		{100,0,0x34,0x2},
		{0,1,0x81,0x1},
		{0,2,0xFD,0x1},
		{0,3,0xCC,0x1},
		{0,4,0x7F,0x2},
		{0,5,0x3,0x2},
		{0,6,0x34,0x2},
		{0,7,0x81,0x1},
		{0,8,0xFD,0x1},
		{0,9,0xCC,0x1},
		{0,10,0x7F,0x2},
		{0,11,0x3,0x2},
		{0xFF, 0xFF, 0xFF, 0xFF}};
		
const guint8 sit[][4] = {
        {100,0,0x95,0x02},
        {0,1,  0xe7,0x00},
        {0,2,  0x2a,0x03},
        {0,3,  0x5f,0x01},
        {0,4,  0x17,0x03},
        {0,5,  0xc4,0x00},
        {0,6,  0xb2,0x02},
        {0,7,  0x00,0x01},
        {0,8,  0x30,0x03},
        {0,9,  0x5c,0x01},
        {0,10, 0x0b,0x03},
        {0,11, 0xc8,0x00},
		{0xFF, 0xFF, 0xFF, 0xFF}};
		
const guint8 walk[][4]={ 
		{40,0,0x34,0x2}, 
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
		{40,0,0x85,0x2}, 
		{0,1,0xC8,0x1}, 
		{0,2,0x38,0x2}, 
		{40,1,0x37,0x1}, 
		{0,2,0x75,0x2}, 
		{40,1,0xF4,0x0}, 
		{0,2,0x38,0x2}, 
		{40,0,0x34,0x2}, 
		{0,1,0x58,0x1}, 
		{0,2,0xB1,0x1}, 
		{40,9,0x7B,0x1}, 
		{0,10,0xC,0x3}, 
		{0,11,0xC8,0x1}, 
		{40,10,0xC9,0x2}, 
		{0,11,0x8B,0x1}, 
		{40,10,0x38,0x2}, 
		{0,11,0xC8,0x1}, 
		{40,9,0xCC,0x1}, 
		{0,10,0xFE,0x1}, 
		{0,11,0x4F,0x2}, 
		{40,1,0x5F,0x1}, 
		{0,2,0xEB,0x1}, 
		{0,4,0x48,0x2}, 
		{0,5,0x15,0x2}, 
		{0,7,0x5F,0x1}, 
		{0,8,0xEB,0x1}, 
		{0,10,0x48,0x2}, 
		{0,11,0x15,0x2}, 
		{40,1,0x81,0x1}, 
		{0,2,0xFD,0x1}, 
		{0,4,0xFE,0x1}, 
		{0,5,0x4F,0x2}, 
		{0,7,0x58,0x1}, 
		{0,8,0xB1,0x1}, 
		{0,10,0x7F,0x2}, 
		{0,11,0x3,0x2}, 
		{40,3,0x7B,0x1}, 
		{0,4,0x38,0x2}, 
		{0,5,0xC8,0x1}, 
		{40,4,0xC9,0x2}, 
		{0,5,0x8B,0x1}, 
		{40,4,0xC,0x3}, 
		{0,5,0xC8,0x1}, 
		{40,3,0xCC,0x1}, 
		{0,4,0xA8,0x2}, 
		{0,5,0x4F,0x2}, 
		{40,6,0x85,0x2}, 
		{0,7,0xF4,0x0}, 
		{0,8,0x38,0x2}, 
		{40,7,0x37,0x1}, 
		{0,8,0x75,0x2}, 
		{40,7,0xC8,0x1}, 
		{0,8,0x38,0x2}, 
		{40,6,0x24,0x2}, 
		/*{40,6,0x34,0x2}, */
		{0,7,0x2,0x2}, 
		{0,8,0xB1,0x1}, 
		{40,1,0xB8,0x1}, 
		{0,2,0xEB,0x1}, 
		{0,4,0xA1,0x2}, 
		{0,5,0x15,0x2}, 
		{0,7,0xB8,0x1}, 
		{0,8,0xEB,0x1}, 
		{0,10,0xA1,0x2}, 
		{0,11,0x15,0x2}, 
		{40,1,0x2,0x2}, 
		{0,2,0xB1,0x1}, 
		{0,4,0x7F,0x2}, 
		{0,5,0x3,0x2}, 
		{0,7,0x81,0x1}, 
		{0,8,0xFD,0x1}, 
		{0,10,0xA8,0x2}, 
		{0,11,0x4F,0x2},
		{0xFF, 0xFF, 0xFF, 0xFF}};
		
const guint8 turnRight1[][4]={
		{100,0,0x34,0x2}, 
		{0,1,0x58,0x1}, 
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
		{100,0,0x45,0x2},
		{0,1,0xE8,0x0},
		{0,2,0x44,0x2},
		{0,3,0xBB,0x1},
		{0,4,0xD1,0x2},
		{0,5,0x7E,0x1},
		{0,6,0x45,0x2},
		{0,7,0x2F,0x1},
		{0,8,0x82,0x2},
		{0,9,0xBB,0x1},
		{0,10,0x18,0x3},
		{0,11,0xBC,0x1},
		{100,0,0x50,0x2},
		{0,1,0x9,0x1},
		{0,2,0xCC,0x1},
		{0,3,0xF0,0x1},
		{0,4,0xA5,0x2},
		{0,5,0x53,0x1},
		{0,6,0x64,0x2},
		{0,7,0x25,0x1},
		{0,8,0x2E,0x2},
		{0,9,0xD4,0x1},
		{0,10,0x1C,0x3},
		{0,11,0x6A,0x1},
		{100,0,0x45,0x2},
		{0,1,0xA3,0x1},
		{0,2,0xD6,0x0},
		{0,3,0xF3,0x1},
		{0,4,0x8B,0x2},
		{0,5,0x8E,0x1},
		{0,6,0x58,0x2},
		{0,7,0x52,0x1},
		{0,8,0xEE,0x1},
		{0,9,0xDB,0x1},
		{0,10,0xEC,0x2},
		{0,11,0xA5,0x1},
		{100,9,0xD4,0x1},
		{0,10,0x1C,0x3},
		{0,11,0x6A,0x1},
		{100,9,0xBB,0x1},
		{0,10,0xD1,0x2},
		{0,11,0x7E,0x1},
		{100,9,0xC4,0x1},
		{0,10,0xAB,0x2},
		{0,11,0xB9,0x1},
		{100,0,0x50,0x2},
		{0,1,0x9,0x1},
		{0,2,0xCC,0x1},
		{100,0,0x45,0x2},
		{0,1,0x2F,0x1},
		{0,2,0x82,0x2},
		{100,0,0x3C,0x2},
		{0,1,0x55,0x1},
		{0,2,0x47,0x2},
		{100,3,0xF0,0x1},
		{0,4,0xA5,0x2},
		{0,5,0x53,0x1},
		{100,3,0xBB,0x1},
		{0,4,0x18,0x3},
		{0,5,0xBC,0x1},
		{100,3,0xC4,0x1},
		{0,4,0xE6,0x2},
		{0,5,0xF8,0x1},
		{100,6,0x64,0x2},
		{0,7,0x25,0x1},
		{0,8,0x2E,0x2},
		{100,6,0x45,0x2},
		{0,7,0xE8,0x0},
		{0,8,0x44,0x2},
		{100,6,0x3C,0x2},
		{0,7,0x1A,0x1},
		{0,8,0x8,0x2},
		{100,0,0x34,0x2},
		{0,1,0x81,0x1},
		{0,2,0xFD,0x1},
		{0,3,0xCC,0x1},
		{0,4,0xA8,0x2},
		{0,5,0x4F,0x2},
		{0,6,0x34,0x2},
		{0,7,0x58,0x1},
		{0,8,0xB1,0x1},
		{0,9,0xCC,0x1},
		{0,10,0x7F,0x2},
		{0,11,0x3,0x2},
		{100,0,0x34,0x2},
		{0,1,0x81,0x1},
		{0,2,0xFD,0x1},
		{0,3,0xCC,0x1},
		{0,4,0x7F,0x2},
		{0,5,0x3,0x2},
		{0,6,0x34,0x2},
		{0,7,0x81,0x1},
		{0,8,0xFD,0x1},
		{0,9,0xCC,0x1},
		{0,10,0x7F,0x2},
		{0,11,0x3,0x2},
		{0xFF, 0xFF, 0xFF, 0xFF}};

