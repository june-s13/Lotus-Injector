#include "mainwindow.h"
#include "injector.h"
#include <gdkmm/display.h>
#include <gtkmm/filechooserdialog.h>
#include <gtkmm/filefilter.h>
#include <gtkmm/cssprovider.h>
#include <gtkmm/stylecontext.h>
#include <gtkmm/application.h>
#include <gtkmm/applicationwindow.h>
#include <gtkmm/box.h>
#include <gtkmm/entry.h>
#include <gtkmm/button.h>
#include <gtkmm/label.h>
#include <gtkmm/messagedialog.h>
#include <gtkmm/popover.h>
#include <iostream>

MainWindow::MainWindow()
: m_DLLInjected(false), m_StatusLabel("Not injected"), m_EnableDisableButton("Enable")
{
    set_title("Lotus Injector");
    set_default_size(600, 400);

    // Apply dark theme
    auto css_provider = Gtk::CssProvider::create();
    css_provider->load_from_data(
        "GtkWindow, GtkScrolledWindow, GtkBox, GtkButton, GtkEntry, GtkLabel, GtkListBox, GtkListBoxRow {"
        "   background-color: #282a36;"
        "   color: #f8f8f2;"
        "   border: none;"
        "}"
        "GtkButton {"
        "   background-color: #6272a4;"
        "   color: #f8f8f2;"
        "   border-radius: 8px;"
        "   border-width: 0px;"
        "   padding: 10px;"
        "}"
        "GtkButton:hover {"
        "   background-color: #bd93f9;"
        "}"
        "GtkLabel {"
        "   color: #f8f8f2;"
        "}"
    );
    auto display = Gdk::Display::get_default();
    Gtk::StyleContext::add_provider_for_display(display, css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);

    // Add the top box for status and logo
    m_HBoxTop.set_spacing(10);
    m_StatusLabel.set_text("DLL injection disabled");
    m_StatusLabel.set_halign(Gtk::Align::START); // Align to the start to avoid extra space
    m_HBoxTop.append(m_StatusLabel);

    // Add the scrolled window and list box for DLLs
    m_ScrolledWindow.set_policy(Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::AUTOMATIC);
    m_ScrolledWindow.set_child(m_DLLListBox);
    m_ScrolledWindow.set_expand(true);

    // Add the buttons
    m_AddDLLButton.set_label("Add DLL");
    m_AddDLLButton.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_add_dll_button_clicked));

    m_EnableDisableButton.set_label("Enable");
    m_EnableDisableButton.set_size_request(100, -1); // Set a fixed width for the button
    m_EnableDisableButton.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_enable_disable_button_clicked));

    m_RemoveButton.set_label("Remove");
    m_RemoveButton.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_remove_button_clicked));

    m_ClearButton.set_label("Clear");
    m_ClearButton.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_clear_button_clicked));

    m_InjectButton.set_label("Inject");
    m_InjectButton.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_inject_button_clicked));

    m_AboutButton.set_label("About");
    m_AboutButton.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_about_button_clicked));

    m_SettingsButton.set_label("Settings");
    m_SettingsButton.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_settings_button_clicked));

    // Add the buttons to the bottom box
    m_HBoxBottom.set_spacing(10);
    m_HBoxBottom.append(m_AddDLLButton);
    m_HBoxBottom.append(m_EnableDisableButton);
    m_HBoxBottom.append(m_RemoveButton);
    m_HBoxBottom.append(m_ClearButton);
    m_HBoxBottom.append(m_InjectButton);
    m_HBoxBottom.append(m_AboutButton);
    m_HBoxBottom.append(m_SettingsButton);

    // Add the top box, scrolled window, and bottom box to the main vertical box
    m_VBox.set_spacing(10);
    m_VBox.set_margin(10);
    m_VBox.append(m_HBoxTop);
    m_VBox.append(m_ScrolledWindow);
    m_VBox.append(m_HBoxBottom);

    // Add the vertical box to the main window
    set_child(m_VBox);

    // Show the window
    show();
}

MainWindow::~MainWindow()
{
}

void MainWindow::on_inject_button_clicked()
{
    if (!m_DLLListBox.get_first_child()) {
        m_StatusLabel.set_text("No DLLs to inject!");
        return;
    }

    Gtk::Widget* child = m_DLLListBox.get_first_child();
    while (child) {
        Gtk::ListBoxRow* row = dynamic_cast<Gtk::ListBoxRow*>(child);
        if (row) {
            Gtk::Widget* row_child = row->get_child();
            Gtk::Label* label = dynamic_cast<Gtk::Label*>(row_child);
            if (label) {
                std::string dllPath = label->get_text();
                DWORD processID = 1234; // Placeholder for actual process ID
                if (CreateRemoteThreadInject(processID, dllPath.c_str())) {
                    m_StatusLabel.set_text("DLL injected successfully!");
                    m_DLLInjected = true;
                } else {
                    m_StatusLabel.set_text("DLL injection failed!");
                    m_DLLInjected = false;
                    return;
                }
            }
        }
        child = child->get_next_sibling();
    }
}

void MainWindow::on_add_dll_button_clicked()
{
    auto dialog = Gtk::make_managed<Gtk::FileChooserDialog>(
        *this, 
        "Please choose a DLL", 
        Gtk::FileChooser::Action::OPEN
    );

    // Add filters
    auto filter_DLL = Gtk::FileFilter::create();
    filter_DLL->set_name("DLL files");
    filter_DLL->add_pattern("*.dll");
    dialog->add_filter(filter_DLL);

    dialog->signal_response().connect(sigc::bind(sigc::mem_fun(*this, &MainWindow::on_file_dialog_response), dialog));
    dialog->show();
}

void MainWindow::on_enable_disable_button_clicked()
{
    if (m_DLLInjected) {
        m_StatusLabel.set_text("DLL injection disabled");
        m_EnableDisableButton.set_label("Enable");
        m_DLLInjected = false;
    } else {
        m_StatusLabel.set_text("DLL injection enabled");
        m_EnableDisableButton.set_label("Disable");
        m_DLLInjected = true;
    }
}

void MainWindow::on_remove_button_clicked()
{
    auto selected_row = m_DLLListBox.get_selected_row();
    if (selected_row) {
        m_DLLListBox.remove(*selected_row);
    }
}

void MainWindow::on_clear_button_clicked()
{
    Gtk::Widget* child = m_DLLListBox.get_first_child();
    while (child) {
        Gtk::Widget* next = child->get_next_sibling();
        m_DLLListBox.remove(*child);
        child = next;
    }
}

void MainWindow::on_about_button_clicked()
{
    auto dialog = Gtk::make_managed<Gtk::MessageDialog>(
        *this, 
        "About Lotus Injector", 
        false, 
        Gtk::MessageType::INFO, 
        Gtk::ButtonsType::OK, 
        true
    );
    dialog->set_secondary_text("Lotus Injector\nVersion 1.0\nDeveloped by [Your Name]");
    dialog->signal_response().connect([dialog](int response_id) {
        dialog->hide();
    });
    dialog->show();
}

void MainWindow::on_settings_button_clicked()
{
    // Create a settings popover
    auto popover = Gtk::make_managed<Gtk::Popover>();

    auto box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 10);
    box->set_margin(10);

    auto injection_type_button = Gtk::make_managed<Gtk::Button>("Injection Type");
    injection_type_button->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_injection_type_button_clicked));

    box->append(*injection_type_button);
    popover->set_child(*box);

    popover->set_position(Gtk::PositionType::BOTTOM);
    popover->set_parent(*this);
    popover->show();
}

void MainWindow::on_injection_type_button_clicked()
{
    // Display available injection types
    auto dialog = Gtk::make_managed<Gtk::MessageDialog>(
        *this, 
        "Select Injection Type", 
        false, 
        Gtk::MessageType::INFO, 
        Gtk::ButtonsType::OK, 
        true
    );
    dialog->set_secondary_text("Available Injection Types:\n1. CreateRemoteThreadInject (current)");
    dialog->signal_response().connect([dialog](int response_id) {
        dialog->hide();
    });
    dialog->show();
}

void MainWindow::on_file_dialog_response(int response_id, Gtk::FileChooserDialog* dialog)
{
    if (response_id == Gtk::ResponseType::ACCEPT) {
        std::string filename = dialog->get_file()->get_path();
        auto* label = Gtk::make_managed<Gtk::Label>(filename);
        auto* row = Gtk::make_managed<Gtk::ListBoxRow>();
        row->set_child(*label);
        m_DLLListBox.append(*row);
        row->show();
    }
}
