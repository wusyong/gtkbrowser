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
    GInputStream *stream;
    gsize         stream_length;

    gchar *contents;
    contents = g_strdup_printf ("<html><body><p>Example html page</p></body></html>");
    stream_length = strlen (contents);
    stream = g_memory_input_stream_new_from_data (contents, stream_length, g_free);

    webkit_uri_scheme_request_finish (request, stream, stream_length, "text/html");
    g_object_unref (stream);
}

int main(int argc, char* argv[])
{
    gtk_init(&argc, &argv);
    gchar *url = "gtk://index.html";
    if (argc == 2) url = argv[1];

    WebKitWebContext *ctx;
    ctx = webkit_web_context_new();
    webkit_web_context_register_uri_scheme(ctx, "gtk", (WebKitURISchemeRequestCallback)uri_scheme_request_cb, NULL, NULL);

    GtkWidget *win;
    WebKitWebView *web;

    win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW(win), 1200, 800);

    web = WEBKIT_WEB_VIEW(webkit_web_view_new_with_context(ctx));

    gtk_container_add(GTK_CONTAINER(win), GTK_WIDGET(web));

    g_signal_connect(win, "destroy", G_CALLBACK(destroy_win_cb), NULL);
    g_signal_connect(web, "close", G_CALLBACK(close_web_cb), win);

    webkit_web_view_load_uri(web, url);

    gtk_widget_grab_focus(GTK_WIDGET(web));
    gtk_widget_show_all(win);
    gtk_main();

    return 0;
}
