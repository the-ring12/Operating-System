#include <gtk/gtk.h>

GtkWidget *label;
GtkWidget *window;
gint count = 1;

// 关闭事件
void destroy(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    gtk_main_quit();
    // g_free(label);
    // g_free(window);
}

// 窗口改变事件
void configure_callback(GtkWindow *window, gpointer data)
{
    int x, y;
    gtk_window_get_size(window, &x, &y);

    // 获取 window 大小，并计算缩放比
    int scale = (x * y) / (600 * 400);
    g_print("%d\n", scale);

    // 设置 label 中 字体大小
    PangoFontDescription *font_desc = pango_font_description_from_string("Sans");
    pango_font_description_set_size(font_desc, 15 * scale * PANGO_SCALE);
    gtk_widget_modify_font(label, font_desc);
}

// 鼠标点击事件
gboolean label_position_color_change(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    // 获取鼠标点击的位置
    gint x = event->x;
    gint y = event->y;
    gint window_x, window_y;
    gtk_window_get_size(widget, &window_x, &window_y);  // 获取窗口大小

    // 判断点击位置，改变 label 的位置
    g_print("%d->%d\n %d -> %d\n", x, y, window_x, window_y);
    if (x < window_x / 2)
    {
        gtk_widget_set_halign(label, GTK_ALIGN_START);
    }
    else
    {
        gtk_widget_set_halign(label, GTK_ALIGN_END);
    }
    if (y < window_y / 2)
    {
        gtk_widget_set_valign(label, GTK_ALIGN_START);
    }
    else
    {
        gtk_widget_set_valign(label, GTK_ALIGN_END);
    }
    
    // 改变text 颜色，点击一次改变一次
    count++;
    g_print("%d", count);
    switch (count  % 4)
    {
    case 1:
        gtk_label_set_markup(GTK_LABEL(label), "<span foreground='red'>Weclcome</span>");
        break;
    case 2:
        gtk_label_set_markup(GTK_LABEL(label), "<span foreground='yellow'>Weclcome</span>");
        break;
    case 3:
        gtk_label_set_markup(GTK_LABEL(label), "<span foreground='blue'>Weclcome</span>");
        count = 0;
        break;
    default:
        break;
    }

    return TRUE;
}

int main(int argc, char *argv[])
{

    gtk_init(&argc, &argv);

    // 窗口设置
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW(window), 600, 400);
    gtk_window_set_title(GTK_WINDOW(window), "Welcome");

    // label 设置
    label = gtk_label_new("Welcome");
    gtk_container_add(GTK_CONTAINER(window), label);

    // 设置 label 初始位置
    gtk_widget_set_valign(label, GTK_ALIGN_CENTER);
    gtk_widget_set_halign(label, GTK_ALIGN_CENTER);

    // 设置 label 初始text大小
    PangoFontDescription *font_desc = pango_font_description_from_string("Sans");
    pango_font_description_set_size(font_desc, 15 * PANGO_SCALE);
    gtk_widget_override_font(label, font_desc);

    // 设置 label 初始 text 颜色
    gtk_label_set_markup(GTK_LABEL(label), "<span foreground='red'>Weclcome</span>");

    // 添加鼠标点击事件
    gtk_widget_add_events(window, GDK_BUTTON_PRESS_MASK);

    // 设置回调
    g_signal_connect(G_OBJECT(window), "delete_event", G_CALLBACK(destroy), window);
    g_signal_connect(G_OBJECT(window), "configure_event", G_CALLBACK(configure_callback), NULL);
    g_signal_connect(G_OBJECT(window), "button-press-event", G_CALLBACK(label_position_color_change), NULL);

    // 显示
    gtk_widget_show_all(window);

    gtk_main();
    return 0;
}