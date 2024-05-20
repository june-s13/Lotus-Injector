#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#pragma warning(push)
#pragma warning(disable : 4250) // Disable dominance warnings

#include <gtkmm.h>

class MainWindow : public Gtk::ApplicationWindow
{
public:
    MainWindow();
    virtual ~MainWindow();

protected:
    // Signal handlers
    void on_inject_button_clicked();
    void on_add_dll_button_clicked();
    void on_enable_disable_button_clicked();
    void on_remove_button_clicked();
    void on_clear_button_clicked();
    void on_about_button_clicked();
    void on_settings_button_clicked();
    void on_injection_type_button_clicked();
    void on_file_dialog_response(int response_id, Gtk::FileChooserDialog* dialog);

    // Member widgets
    Gtk::Box m_VBox{Gtk::Orientation::VERTICAL};
    Gtk::Box m_HBoxTop{Gtk::Orientation::HORIZONTAL};
    Gtk::Box m_HBoxBottom{Gtk::Orientation::HORIZONTAL};
    Gtk::ScrolledWindow m_ScrolledWindow;
    Gtk::ListBox m_DLLListBox;
    Gtk::Label m_StatusLabel;
    Gtk::Button m_AddDLLButton;
    Gtk::Button m_EnableDisableButton;
    Gtk::Button m_RemoveButton;
    Gtk::Button m_ClearButton;
    Gtk::Button m_InjectButton;
    Gtk::Button m_AboutButton;
    Gtk::Button m_SettingsButton;

    bool m_DLLInjected;
};

#pragma warning(pop)

#endif // MAINWINDOW_H
