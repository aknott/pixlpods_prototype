
#define RED        0xFF0000
#define ORANGE     0xFF7F00
#define YELLOW     0xFFFF00
#define CHARTRUESE 0x7FFF00
#define GREEN      0x00FF00
#define SPRING     0x00FF7F
#define CYAN       0x00FFFF
#define AZURE      0x007FFF
#define BLUE       0x0000FF
#define VIOLET     0x7F00FF
#define MAGENTA    0xFF00FF
#define ROSE       0xFF007F
#define WHITE      0xFFFFFF

uint32_t WHEEL[12] = {
  RED,
  ORANGE,
  YELLOW,
  CHARTRUESE,
  GREEN,
  SPRING,
  CYAN,
  AZURE,
  BLUE,
  VIOLET,
  MAGENTA,
  ROSE
};

uint32_t earth_palette[4] = {
  GREEN,
  0x00FF1F,
  AZURE,
  WHITE
};

uint32_t fire_palette[4] = {
  RED,
  YELLOW,
  ORANGE,
  ROSE
};

uint32_t water_palatte[3] = {
  CYAN,
  BLUE,
  AZURE
};

struct palette {
  uint8_t num_colors;
  uint32_t *colors;
};

uint32_t christmas_colors[2] = {
  RED,
  GREEN
};

struct palette CHRISTMAS = { 2, christmas_colors };

uint32_t july_fourth_colors[3] = {
  RED,
  WHITE,
  BLUE
};

struct palette JULYFOURTH = { 3, july_fourth_colors };

uint32_t halloween_colors[1] = {
  0xFF1F00
};


struct palette HALLOWEEN = { 1, halloween_colors };

uint32_t mardi_gras_colors[3] = {
  SPRING,
  YELLOW,
  VIOLET
};


struct palette MARDIGRAS = { 3, mardi_gras_colors };

struct palette HOLIDAYS[4] = {
  CHRISTMAS,
  HALLOWEEN,
  JULYFOURTH,
  MARDIGRAS
};
