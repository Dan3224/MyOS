#include "graphics.h"
#include "console.h"

#define MULTIBOOT_BOOTLOADER_MAGIC 0x2BADB002
#define MULTIBOOT_INFO_FRAMEBUFFER 0x00001000
#define MULTIBOOT_FRAMEBUFFER_RGB 1

#define RGB(r, g, b) (((unsigned int)(r) << 16) | ((unsigned int)(g) << 8) | (unsigned int)(b))

struct multiboot_info {
    unsigned int flags;
    unsigned int mem_lower;
    unsigned int mem_upper;
    unsigned int boot_device;
    unsigned int cmdline;
    unsigned int mods_count;
    unsigned int mods_addr;
    unsigned int symbol_table[4];
    unsigned int mmap_length;
    unsigned int mmap_addr;
    unsigned int drives_length;
    unsigned int drives_addr;
    unsigned int config_table;
    unsigned int boot_loader_name;
    unsigned int apm_table;
    unsigned int vbe_control_info;
    unsigned int vbe_mode_info;
    unsigned short vbe_mode;
    unsigned short vbe_interface_seg;
    unsigned short vbe_interface_off;
    unsigned short vbe_interface_len;
    unsigned int framebuffer_addr_low;
    unsigned int framebuffer_addr_high;
    unsigned int framebuffer_pitch;
    unsigned int framebuffer_width;
    unsigned int framebuffer_height;
    unsigned char framebuffer_bpp;
    unsigned char framebuffer_type;
    unsigned char framebuffer_red_position;
    unsigned char framebuffer_red_mask_size;
    unsigned char framebuffer_green_position;
    unsigned char framebuffer_green_mask_size;
    unsigned char framebuffer_blue_position;
    unsigned char framebuffer_blue_mask_size;
} __attribute__((packed));

static unsigned int *framebuffer;
static unsigned int framebuffer_width;
static unsigned int framebuffer_height;
static unsigned int framebuffer_pitch;
static unsigned char red_position;
static unsigned char red_mask_size;
static unsigned char green_position;
static unsigned char green_mask_size;
static unsigned char blue_position;
static unsigned char blue_mask_size;

static const unsigned char alphabet[26][7] = {
    {0x0E,0x11,0x11,0x1F,0x11,0x11,0x11}, {0x1E,0x11,0x11,0x1E,0x11,0x11,0x1E},
    {0x0E,0x11,0x10,0x10,0x10,0x11,0x0E}, {0x1E,0x11,0x11,0x11,0x11,0x11,0x1E},
    {0x1F,0x10,0x10,0x1E,0x10,0x10,0x1F}, {0x1F,0x10,0x10,0x1E,0x10,0x10,0x10},
    {0x0E,0x11,0x10,0x17,0x11,0x11,0x0F}, {0x11,0x11,0x11,0x1F,0x11,0x11,0x11},
    {0x1F,0x04,0x04,0x04,0x04,0x04,0x1F}, {0x07,0x02,0x02,0x02,0x02,0x12,0x0C},
    {0x11,0x12,0x14,0x18,0x14,0x12,0x11}, {0x10,0x10,0x10,0x10,0x10,0x10,0x1F},
    {0x11,0x1B,0x15,0x15,0x11,0x11,0x11}, {0x11,0x19,0x15,0x13,0x11,0x11,0x11},
    {0x0E,0x11,0x11,0x11,0x11,0x11,0x0E}, {0x1E,0x11,0x11,0x1E,0x10,0x10,0x10},
    {0x0E,0x11,0x11,0x11,0x15,0x12,0x0D}, {0x1E,0x11,0x11,0x1E,0x14,0x12,0x11},
    {0x0F,0x10,0x10,0x0E,0x01,0x01,0x1E}, {0x1F,0x04,0x04,0x04,0x04,0x04,0x04},
    {0x11,0x11,0x11,0x11,0x11,0x11,0x0E}, {0x11,0x11,0x11,0x11,0x11,0x0A,0x04},
    {0x11,0x11,0x11,0x15,0x15,0x15,0x0A}, {0x11,0x11,0x0A,0x04,0x0A,0x11,0x11},
    {0x11,0x11,0x0A,0x04,0x04,0x04,0x04}, {0x1F,0x01,0x02,0x04,0x08,0x10,0x1F}
};

static const unsigned char digits[10][7] = {
    {0x0E,0x11,0x13,0x15,0x19,0x11,0x0E}, {0x04,0x0C,0x04,0x04,0x04,0x04,0x0E},
    {0x0E,0x11,0x01,0x02,0x04,0x08,0x1F}, {0x1E,0x01,0x01,0x0E,0x01,0x01,0x1E},
    {0x02,0x06,0x0A,0x12,0x1F,0x02,0x02}, {0x1F,0x10,0x10,0x1E,0x01,0x01,0x1E},
    {0x0E,0x10,0x10,0x1E,0x11,0x11,0x0E}, {0x1F,0x01,0x02,0x04,0x08,0x08,0x08},
    {0x0E,0x11,0x11,0x0E,0x11,0x11,0x0E}, {0x0E,0x11,0x11,0x0F,0x01,0x01,0x0E}
};

static unsigned int color(unsigned int rgb) {
    unsigned int red = (rgb >> 16) & 0xFF;
    unsigned int green = (rgb >> 8) & 0xFF;
    unsigned int blue = rgb & 0xFF;

    red >>= 8 - red_mask_size;
    green >>= 8 - green_mask_size;
    blue >>= 8 - blue_mask_size;

    return (red << red_position) | (green << green_position) | (blue << blue_position);
}

static void fill_rect(unsigned int x, unsigned int y, unsigned int width,
                      unsigned int height, unsigned int rgb) {
    unsigned int packed = color(rgb);

    if (!framebuffer) {
        return;
    }
    if (x >= framebuffer_width || y >= framebuffer_height) {
        return;
    }
    if (x + width > framebuffer_width) {
        width = framebuffer_width - x;
    }
    if (y + height > framebuffer_height) {
        height = framebuffer_height - y;
    }

    for (unsigned int row = 0; row < height; row++) {
        unsigned int *line = (unsigned int *)((unsigned char *)framebuffer +
                                              (y + row) * framebuffer_pitch);
        for (unsigned int column = 0; column < width; column++) {
            line[x + column] = packed;
        }
    }
}

static void outline_rect(unsigned int x, unsigned int y, unsigned int width,
                         unsigned int height, unsigned int rgb) {
    fill_rect(x, y, width, 2, rgb);
    fill_rect(x, y + height - 2, width, 2, rgb);
    fill_rect(x, y, 2, height, rgb);
    fill_rect(x + width - 2, y, 2, height, rgb);
}

static unsigned char glyph_row(char character, unsigned int row) {
    if (character >= 'a' && character <= 'z') {
        character -= 'a' - 'A';
    }
    if (character >= 'A' && character <= 'Z') {
        return alphabet[character - 'A'][row];
    }
    if (character >= '0' && character <= '9') {
        return digits[character - '0'][row];
    }
    if (character == '-') {
        return row == 3 ? 0x1F : 0;
    }
    if (character == '.') {
        return row == 6 ? 0x04 : 0;
    }
    if (character == ':') {
        return row == 2 || row == 5 ? 0x04 : 0;
    }
    if (character == '/') {
        return 1U << (row > 4 ? 0 : 4 - row);
    }
    return 0;
}

static void draw_char(unsigned int x, unsigned int y, char character,
                      unsigned int rgb, unsigned int scale) {
    for (unsigned int row = 0; row < 7; row++) {
        unsigned char bits = glyph_row(character, row);
        for (unsigned int column = 0; column < 5; column++) {
            if (bits & (1U << (4 - column))) {
                fill_rect(x + column * scale, y + row * scale, scale, scale, rgb);
            }
        }
    }
}

static void draw_text(unsigned int x, unsigned int y, const char *text,
                      unsigned int rgb, unsigned int scale) {
    while (*text) {
        draw_char(x, y, *text++, rgb, scale);
        x += 6 * scale;
    }
}

static void draw_card(unsigned int x, unsigned int y, unsigned int width,
                      unsigned int height, unsigned int accent,
                      const char *title, const char *description) {
    fill_rect(x, y, width, height, RGB(27, 36, 58));
    outline_rect(x, y, width, height, accent);
    fill_rect(x, y, 8, height, accent);
    draw_text(x + 28, y + 28, title, RGB(240, 244, 255), 3);
    draw_text(x + 28, y + 78, description, RGB(159, 174, 204), 2);
}

static void draw_top_bar(const char *page) {
    fill_rect(0, 0, framebuffer_width, 72, RGB(16, 23, 40));
    fill_rect(0, 70, framebuffer_width, 2, RGB(103, 183, 255));
    draw_text(32, 22, "MYOS 1.0", RGB(255, 255, 255), 3);
    draw_text(260, 26, page, RGB(103, 183, 255), 2);
    draw_text(framebuffer_width - 160, 26, "SYSTEM LIVE", RGB(100, 240, 170), 2);
}

static void draw_footer(void) {
    fill_rect(0, framebuffer_height - 62, framebuffer_width, 62, RGB(16, 23, 40));
    draw_text(30, framebuffer_height - 41,
              "H HOME  F FILES  S SYSTEM  A APPS  N NOTES  Q REBOOT",
              RGB(180, 195, 220), 2);
}

static void draw_desktop(const char *page) {
    fill_rect(0, 0, framebuffer_width, framebuffer_height, RGB(13, 20, 36));
    draw_top_bar(page);
    draw_text(50, 125, "WELCOME TO MYOS", RGB(240, 244, 255), 4);
    draw_text(52, 180, "A SMALL SYSTEM WITH A BIG DIRECTION", RGB(159, 174, 204), 2);
    draw_card(50, 255, 430, 150, RGB(103, 183, 255), "F FILES", "VIRTUAL STORAGE");
    draw_card(545, 255, 430, 150, RGB(100, 240, 170), "S SYSTEM", "HEALTH AND STATUS");
    draw_card(50, 450, 430, 150, RGB(255, 190, 90), "A APPS", "OPEN APP LAUNCHER");
    draw_card(545, 450, 430, 150, RGB(206, 130, 255), "N NOTES", "WRITE IDEAS LATER");
    draw_footer();
}

static void draw_window(unsigned int x, unsigned int y, unsigned int width,
                        unsigned int height, unsigned int accent,
                        const char *title) {
    fill_rect(x + 10, y + 10, width, height, RGB(7, 11, 21));
    fill_rect(x, y, width, height, RGB(24, 32, 52));
    outline_rect(x, y, width, height, accent);
    fill_rect(x, y, width, 54, RGB(18, 25, 43));
    fill_rect(x, y, 10, 54, accent);
    fill_rect(x + width - 24, y + 18, 10, 10, RGB(255, 125, 145));
    draw_text(x + 30, y + 18, title, RGB(240, 244, 255), 3);
    draw_text(x + width - 145, y + 20, "H CLOSE", RGB(180, 195, 220), 2);
}

int graphics_init(unsigned int multiboot_magic, unsigned int multiboot_info) {
    struct multiboot_info *info = (struct multiboot_info *)multiboot_info;

    if (multiboot_magic != MULTIBOOT_BOOTLOADER_MAGIC) {
        return -1;
    }
    if ((info->flags & MULTIBOOT_INFO_FRAMEBUFFER) == 0) {
        return -2;
    }
    if (info->framebuffer_addr_high != 0 ||
        info->framebuffer_bpp != 32 ||
        info->framebuffer_type != MULTIBOOT_FRAMEBUFFER_RGB) {
        return -3;
    }

    framebuffer = (unsigned int *)info->framebuffer_addr_low;
    framebuffer_width = info->framebuffer_width;
    framebuffer_height = info->framebuffer_height;
    framebuffer_pitch = info->framebuffer_pitch;
    red_position = info->framebuffer_red_position;
    red_mask_size = info->framebuffer_red_mask_size;
    green_position = info->framebuffer_green_position;
    green_mask_size = info->framebuffer_green_mask_size;
    blue_position = info->framebuffer_blue_position;
    blue_mask_size = info->framebuffer_blue_mask_size;

    if (framebuffer_width < 640 || framebuffer_height < 480 ||
        framebuffer_pitch < framebuffer_width * 4) {
        framebuffer = 0;
        return -4;
    }
    if (red_mask_size == 0 || green_mask_size == 0 || blue_mask_size == 0) {
        red_position = 16;
        red_mask_size = 8;
        green_position = 8;
        green_mask_size = 8;
        blue_position = 0;
        blue_mask_size = 8;
    }

    return 1;
}

void graphics_show_boot_stage(unsigned int stage) {
    static const char *stages[4] = {
        "BOOT LOADER CONNECTED",
        "VIDEO DRIVER READY",
        "INPUT DRIVER READY",
        "SYSTEM READY"
    };

    fill_rect(0, 0, framebuffer_width, framebuffer_height, RGB(10, 15, 28));
    fill_rect(0, 0, framebuffer_width, 10, RGB(103, 183, 255));
    fill_rect(0, framebuffer_height - 10, framebuffer_width, 10, RGB(103, 183, 255));

    fill_rect(framebuffer_width / 2 - 150, 130, 300, 300, RGB(26, 43, 76));
    outline_rect(framebuffer_width / 2 - 150, 130, 300, 300, RGB(103, 183, 255));
    fill_rect(framebuffer_width / 2 - 90, 195, 35, 160, RGB(103, 183, 255));
    fill_rect(framebuffer_width / 2 - 45, 235, 35, 120, RGB(150, 220, 255));
    fill_rect(framebuffer_width / 2, 195, 35, 160, RGB(103, 183, 255));
    fill_rect(framebuffer_width / 2 + 45, 235, 35, 120, RGB(150, 220, 255));

    draw_text(framebuffer_width / 2 - 135, 470, "MYOS", RGB(240, 244, 255), 6);
    draw_text(framebuffer_width / 2 - 160, 545, "1.0 GRAPHICAL CORE", RGB(103, 183, 255), 3);

    for (unsigned int index = 0; index < 4; index++) {
        unsigned int y = 635 + index * 34;
        unsigned int status_color = index <= stage ? RGB(100, 240, 170) : RGB(70, 86, 117);
        fill_rect(framebuffer_width / 2 - 190, y + 2, 16, 16, status_color);
        draw_text(framebuffer_width / 2 - 150, y, stages[index],
                  index <= stage ? RGB(220, 235, 255) : RGB(105, 120, 150), 2);
    }
}

void graphics_show_home(void) {
    draw_desktop("HOME");
}

void graphics_show_files(unsigned int file_count) {
    draw_desktop("DESKTOP");
    draw_window(110, 130, 804, 520, RGB(103, 183, 255), "FILES");
    draw_text(150, 220, "VIRTUAL STORAGE", RGB(159, 174, 204), 2);
    draw_card(150, 255, 720, 100, RGB(103, 183, 255), "README", "WELCOME TO MYOS");
    draw_card(150, 370, 720, 100, RGB(100, 240, 170), "VERSION", "MYOS 1.0 GRAPHICAL CORE");
    draw_card(150, 485, 720, 100, RGB(255, 190, 90), "FILES",
              file_count > 2 ? "USER FILES PRESENT" : "READY FOR USER FILES");
}

void graphics_show_system(unsigned long uptime_seconds, unsigned int file_count) {
    (void)uptime_seconds;
    (void)file_count;
    draw_desktop("DESKTOP");
    draw_window(110, 130, 804, 520, RGB(100, 240, 170), "SYSTEM");
    draw_text(150, 220, "SYSTEM STATUS", RGB(159, 174, 204), 2);
    draw_card(150, 255, 720, 100, RGB(100, 240, 170), "KERNEL", "RUNNING WITH TIMER INTERRUPTS");
    draw_card(150, 370, 720, 100, RGB(103, 183, 255), "DISPLAY", "32 BIT FRAMEBUFFER ACTIVE");
    draw_card(150, 485, 720, 100, RGB(206, 130, 255), "HEALTH", "HEARTBEAT LIGHT IN TOP BAR");
}

void graphics_show_apps(void) {
    draw_desktop("DESKTOP");
    draw_window(110, 130, 804, 520, RGB(255, 190, 90), "APP LAUNCHER");
    draw_text(150, 220, "CHOOSE A SPACE TO EXPLORE", RGB(159, 174, 204), 2);
    draw_card(150, 280, 325, 115, RGB(103, 183, 255), "F FILES", "BROWSE STORAGE");
    draw_card(535, 280, 325, 115, RGB(100, 240, 170), "S SYSTEM", "CHECK HEALTH");
    draw_card(150, 440, 325, 115, RGB(206, 130, 255), "N NOTES", "DRAFT SPACE");
    draw_card(535, 440, 325, 115, RGB(255, 190, 90), "TOOLS", "COMING NEXT");
}

void graphics_show_notes(const char *text, int editing) {
    unsigned int x = 155;
    unsigned int y = 335;

    draw_desktop("DESKTOP");
    draw_window(110, 130, 804, 520, RGB(206, 130, 255), "NOTES");
    if (editing) {
        draw_text(155, 235, "EDIT MODE - ENTER SAVES", RGB(220, 205, 255), 3);
    } else {
        draw_text(155, 235, "PRESS E TO EDIT - H CLOSES", RGB(220, 205, 255), 3);
    }

    fill_rect(150, 300, 724, 265, RGB(18, 25, 43));
    outline_rect(150, 300, 724, 265, RGB(76, 63, 110));

    while (*text && y < 525) {
        if (x > 830) {
            x = 155;
            y += 32;
        }
        draw_char(x, y, *text++, RGB(240, 244, 255), 3);
        x += 18;
    }

    if (!text[0] && x == 155) {
        draw_text(155, 335, "NO NOTE SAVED YET", RGB(159, 174, 204), 2);
    }
}

void graphics_show_crash(unsigned int vector) {
    const char *fault = "UNKNOWN EXCEPTION";

    if (!framebuffer) {
        console_clear(0x4F);
        console_write("MyOS recovery screen\nKernel exception. Restart the virtual machine.\n");
        return;
    }

    if (vector == 0) {
        fault = "DIVIDE BY ZERO";
    } else if (vector == 6) {
        fault = "INVALID OPCODE";
    } else if (vector == 13) {
        fault = "GENERAL PROTECTION";
    } else if (vector == 14) {
        fault = "PAGE FAULT";
    }

    fill_rect(0, 0, framebuffer_width, framebuffer_height, RGB(48, 12, 28));
    fill_rect(0, 0, framebuffer_width, 12, RGB(255, 114, 145));
    fill_rect(0, framebuffer_height - 12, framebuffer_width, 12, RGB(255, 114, 145));
    draw_text(65, 115, "MYOS RECOVERY SCREEN", RGB(255, 240, 244), 4);
    draw_text(70, 230, "KERNEL EXCEPTION", RGB(255, 160, 180), 3);
    draw_text(70, 305, fault, RGB(255, 255, 255), 4);
    draw_text(70, 430, "THE SYSTEM STOPPED SAFELY", RGB(255, 220, 230), 2);
    draw_text(70, 470, "RESTART QEMU TO CONTINUE", RGB(255, 220, 230), 2);
}

void graphics_update_heartbeat(unsigned long ticks) {
    unsigned int heartbeat_color = (ticks / 25) % 2 ? RGB(100, 240, 170) : RGB(35, 100, 70);
    fill_rect(framebuffer_width - 35, 25, 18, 18, heartbeat_color);
}
