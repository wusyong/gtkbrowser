#include <gtk/gtk.h>
#include <webkit2/webkit2.h>

static void destroy_win_cb(GtkWidget* widget, GtkWidget* window)
{
    gtk_main_quit();
}

static gboolean close_web_cb(WebKitWebView* webView, GtkWidget* window)
{
    gtk_widget_destroy(window);
    return TRUE;
}

static void uri_scheme_request_cb (WebKitURISchemeRequest *request, gpointer user_data)
{
    // TODO Better video file
    char path[256] = "/home/wusyong/gtkbrowser/test.webm";
    GFile *file;
    GFileInputStream *stream;
    GError *err = NULL;
    gsize stream_length = 2165175;

    file = g_file_new_for_path (path);
    stream = g_file_read (file, NULL, &err);

    if (err != NULL)
    {
        g_error ("Could not open %s for reading: %s\n", path, err->message);
        g_error_free (err);
    }

    webkit_uri_scheme_request_finish (request, G_INPUT_STREAM(stream), stream_length, "video/webm");
    g_object_unref (stream);
}

int main(int argc, char* argv[])
{
    gtk_init(&argc, &argv);
    gchar *url = "gtk://test_video.mp4";

    WebKitWebContext *ctx;
    ctx = webkit_web_context_new();
    webkit_web_context_register_uri_scheme(ctx, "gtk", (WebKitURISchemeRequestCallback)uri_scheme_request_cb, NULL, NULL);

    GtkWidget *win;
    WebKitWebView *web;
    WebKitSettings *settings; 

    win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW(win), 1200, 800);

    web = WEBKIT_WEB_VIEW(webkit_web_view_new_with_context(ctx));
    settings = webkit_web_view_get_settings(web);
    webkit_settings_set_enable_developer_extras(settings, true);

    gtk_container_add(GTK_CONTAINER(win), GTK_WIDGET(web));

    g_signal_connect(win, "destroy", G_CALLBACK(destroy_win_cb), NULL);
    g_signal_connect(web, "close", G_CALLBACK(close_web_cb), win);

    webkit_web_view_load_uri(web, url);

    gtk_widget_grab_focus(GTK_WIDGET(web));
    gtk_widget_show_all(win);
    gtk_main();

    return 0;
}
