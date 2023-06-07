#include <map>
#include <gtk/gtk.h>
#include "graphics-info.h"
#include "c-interface-gtk-widgets.h"
#include "setup-gui-components.hh"
#include "widget-from-builder.hh"

// this function is both defined and implemented here.
// No other files should ever need it.
inline GMenuModel* menu_model_from_builder(const std::string& m_name) {
   GMenuModel *m = G_MENU_MODEL(graphics_info_t::get_gobject_from_builder(m_name));
   return m;
}

// this function is both defined and implemented here.
// No other files should ever need it.
inline GMenu* menu_from_builder(const std::string& m_name) {
   GMenu *m = G_MENU(graphics_info_t::get_gobject_from_builder(m_name));
   return m;
}

void setup_menubuttons() {
   GtkWidget* add_module_menubutton = widget_from_builder("add_module_menubutton");
   GMenuModel *modules_menu = menu_model_from_builder("modules-menu");
   gtk_menu_button_set_menu_model(GTK_MENU_BUTTON(add_module_menubutton), modules_menu);

   // toolbar button - connect the refine menu to the GtkMenuButton
   GtkWidget *refine_menubutton = widget_from_builder("refine_menubutton");
   GMenuModel *refine_menu = menu_model_from_builder("refine-menu");
   gtk_menu_button_set_menu_model(GTK_MENU_BUTTON(refine_menubutton), refine_menu);

   GtkWidget *fixed_atoms_menubutton = widget_from_builder("fixed_atoms_menubutton");
   GMenuModel *fixed_atoms_menu = menu_model_from_builder("fixed-atoms-menu");
   gtk_menu_button_set_menu_model(GTK_MENU_BUTTON(fixed_atoms_menubutton), fixed_atoms_menu);

   GtkWidget *delete_menubutton = widget_from_builder("delete_menubutton");
   GMenuModel *delete_item_menu = menu_model_from_builder("delete-item-menu");
   gtk_menu_button_set_menu_model(GTK_MENU_BUTTON(delete_menubutton), delete_item_menu);

   // move this function to where it can be called when we click on the "Mutate"
   // button (both of them, I suppose).
   auto add_typed_menu_to_mutate_menubutton = [] (const std::string &residue_type) {
      if (residue_type == "PROTEIN") {
         GtkWidget *mutate_menubutton = widget_from_builder("simple_mutate_menubutton");
         GMenuModel *mutate_menu = menu_model_from_builder("mutate-protein-menu");
         gtk_menu_button_set_menu_model(GTK_MENU_BUTTON(mutate_menubutton), mutate_menu);

         mutate_menubutton = widget_from_builder("mutate_and_autofit_menubutton");
         gtk_menu_button_set_menu_model(GTK_MENU_BUTTON(mutate_menubutton), mutate_menu);
      }
      if (residue_type == "NUCLEIC-ACID") {
         GtkWidget *mutate_menubutton = widget_from_builder("simple_mutate_menubutton");
         GMenuModel *mutate_menu = menu_model_from_builder("mutate-nucleic-acid-menu");
         gtk_menu_button_set_menu_model(GTK_MENU_BUTTON(mutate_menubutton), mutate_menu);

         mutate_menubutton = widget_from_builder("mutate_and_autofit_menubutton");
         gtk_menu_button_set_menu_model(GTK_MENU_BUTTON(mutate_menubutton), mutate_menu);
      }
   };


   add_typed_menu_to_mutate_menubutton("PROTEIN");
}

gboolean generic_hide_on_escape_controller_cb(GtkEventControllerKey  *controller,
                                              guint                  keyval,
                                              guint                  keycode,
                                              GdkModifierType        modifiers,
                                              GtkWidget              *to_be_hidden) {
   gboolean handled = TRUE;
   switch (keyval) {
      case GDK_KEY_Escape: {
         gtk_widget_hide(to_be_hidden);
         break;
      }
      default: {
         g_debug("generic_hide_on_escape_controller_cb: unhandled key: %s",gdk_keyval_name(keyval));
         handled = FALSE;
         break;
      }
   }
   return gboolean(handled);
}

void setup_generic_hide_on_escape_controller(GtkWidget* target_widget, GtkWidget* to_be_hidden) {
   GtkEventController *key_controller = gtk_event_controller_key_new();
   g_debug("Setting up hide-on-Escape controller on %p to hide %p",target_widget,to_be_hidden);
   g_signal_connect(key_controller, "key-pressed",G_CALLBACK(generic_hide_on_escape_controller_cb), to_be_hidden);
   gtk_widget_add_controller(target_widget, key_controller);
}

void setup_accession_code_frame() {
   GtkWidget *frame = widget_from_builder("accession_code_frame");
   GtkWidget* entry = widget_from_builder("accession_code_entry");
   g_signal_connect(entry,"activate",G_CALLBACK(+[](GtkEntry* entry, gpointer user_data){
      GtkWidget* frame = GTK_WIDGET(user_data);
      handle_get_accession_code(frame, GTK_WIDGET(entry));
   }),frame);
   setup_generic_hide_on_escape_controller(entry,frame);
}

void setup_validation_graph_dialog() {

   GtkWidget *model_combobox = widget_from_builder("validation_graph_model_combobox");
   gtk_combo_box_set_model(GTK_COMBO_BOX(model_combobox),GTK_TREE_MODEL(graphics_info_t::validation_graph_model_list));
   gtk_combo_box_set_id_column(GTK_COMBO_BOX(model_combobox),0);

}

void setup_ramachandran_plot_chooser_dialog() {

   GtkWidget *model_combobox = widget_from_builder("ramachandran_plot_molecule_chooser_model_combobox");
   gtk_combo_box_set_model(GTK_COMBO_BOX(model_combobox), GTK_TREE_MODEL(graphics_info_t::ramachandran_plot_model_list));
   gtk_combo_box_set_id_column(GTK_COMBO_BOX(model_combobox),0);

}

void setup_get_monomer() {

   GtkWidget* frame = widget_from_builder("get_monomer_frame");
   GtkWidget* entry = widget_from_builder("get_monomer_entry");
   g_signal_connect(entry,"activate",G_CALLBACK(+[](GtkEntry* entry, gpointer user_data){
      handle_get_monomer_code(GTK_WIDGET(entry));
   }),NULL);
   setup_generic_hide_on_escape_controller(entry,frame);
}

void attach_css_style_class_to_overlays() {

   GtkCssProvider *provider = gtk_css_provider_new();
   gtk_css_provider_load_from_data (provider, ".mainWindowOverlayChild { background: rgba(0,0,0,0.7); }", -1);

   auto set_transparency_on_widget = [provider](GtkWidget* widget){
      GtkStyleContext *context = gtk_widget_get_style_context(widget);
      gtk_style_context_add_provider (context, GTK_STYLE_PROVIDER (provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
      gtk_style_context_add_class (context, "mainWindowOverlayChild");
      g_debug("'mainWindowOverlayChild' CSS class set for: %p",widget);
   };

   GtkWidget* overlay = widget_from_builder("main_window_graphics_overlay");
   GtkWidget* to_skip = widget_from_builder("main_window_graphics_hbox");
   for (GtkWidget* child = gtk_widget_get_first_child(overlay);
       child != nullptr; 
       child = gtk_widget_get_next_sibling(child)) {
       if (child != to_skip)
          set_transparency_on_widget(child);
   }
}


void
add_python_scripting_entry_completion(GtkWidget *entry) {

   // call this *after* python has been setup!

   graphics_info_t g; // for history

   GtkEntryCompletion *completion = gtk_entry_completion_new();
   gtk_entry_completion_set_popup_completion(completion, TRUE);
   gtk_entry_completion_set_text_column(completion, 0);
   gtk_entry_completion_set_minimum_key_length(completion, 2);
   gtk_entry_set_completion(GTK_ENTRY(entry), completion);

   std::vector<std::string> completions;
   std::vector<std::string> module_coot_completions;
   std::vector<std::string> module_coot_utils_completions;

   PyErr_Clear();

   Py_ssize_t pos = 0;
   PyObject *key;
   PyObject *value;

   // Get the module object for the `sys` module.
   PyObject *module = PyImport_ImportModule("coot");
   // Get the dictionary object for the `sys` module.
   PyObject *dict = PyModule_GetDict(module);
  // Iterate over the keys and values in the dictionary.
   while (PyDict_Next(dict, &pos, &key, &value)) {
      // Do something interesting with the key and value.
      // printf("Key: %s, Value: %s\n", PyUnicode_AsUTF8AndSize(key, NULL), PyUnicode_AsUTF8AndSize(value, NULL));
      std::string key_c = std::string("coot.") +  (PyUnicode_AsUTF8AndSize(key, NULL));
      module_coot_completions.push_back(key_c);
   }
   // Get the module object for the `sys` module.
   module = PyImport_ImportModule("coot_utils");
   // Get the dictionary object for the `sys` module.
   dict = PyModule_GetDict(module);
  // Iterate over the keys and values in the dictionary.
   while (PyDict_Next(dict, &pos, &key, &value)) {
      // Do something interesting with the key and value.
      // printf("Key: %s, Value: %s\n", PyUnicode_AsUTF8AndSize(key, NULL), PyUnicode_AsUTF8AndSize(value, NULL));
      std::string key_c = std::string("coot_utils.") +  (PyUnicode_AsUTF8AndSize(key, NULL));
      module_coot_utils_completions.push_back(key_c);
   }

   // command history
   std::vector<std::string> chv = g.command_history.commands;

   chv = g.command_history.unique_commands(); // there *were* unique already

   if (false) chv.clear(); // 20230516-PE while testing.

   // add together the completions
   completions.push_back("import coot");
   completions.push_back("import coot_utils");
   completions.insert(completions.end(), chv.begin(),                           chv.end());
   completions.insert(completions.end(), module_coot_completions.begin(),       module_coot_completions.end());
   completions.insert(completions.end(), module_coot_utils_completions.begin(), module_coot_utils_completions.end());

   // maybe only once!
   GtkListStore *store = gtk_list_store_new(1, G_TYPE_STRING);
   GtkTreeIter iter;

   for (unsigned int i=0; i<completions.size(); i++) {
      gtk_list_store_append( store, &iter );
      std::string c = completions[i];
      // std::cout << "adding to gtk-completion: " << c << std::endl;
      gtk_list_store_set( store, &iter, 0, c.c_str(), -1 );
   }

   gtk_entry_completion_set_model(completion, GTK_TREE_MODEL(store));

}


gboolean
on_python_scripting_entry_key_pressed(GtkEventControllerKey *controller,
                                      guint                  keyval,
                                      guint                  keycode,
                                      GdkModifierType        modifiers,
                                      GtkEntry              *entry) {

   // This function is called on Ctrl and Shift, and Arrowkey Up and Down key presses

   gboolean handled = TRUE;
   bool control_is_pressed = (modifiers & GDK_CONTROL_MASK);

   std::cout << "on_python_scripting_entry_key_pressed() keyval: " << keyval << " keycode: " << keycode << std::endl;

   switch(keyval) {
      case GDK_KEY_Up: {
         handled = TRUE;
         if (control_is_pressed) {
            graphics_info_t g;
            std::string t = g.command_history.get_previous_command();
            gtk_editable_set_text(GTK_EDITABLE(entry), t.c_str());
         }
         break;
      }
      case GDK_KEY_Down: {
         handled = TRUE;
         if (control_is_pressed) {
            graphics_info_t g;
            std::string t = g.command_history.get_next_command();
            gtk_editable_set_text(GTK_EDITABLE(entry), t.c_str());
         }
         break;
      }
      case GDK_KEY_Escape: {
         auto func = +[] (gpointer data) {
            GtkRevealer* revealer = GTK_REVEALER(widget_from_builder("python_scripting_revealer"));
            gtk_revealer_set_reveal_child(revealer,FALSE);
            return gboolean(G_SOURCE_REMOVE);
         };
         g_idle_add(func, NULL);
         break;
      }
      default: {
         handled = FALSE;
         g_debug("Python scripting entry: Unhandled key: %s",gdk_keyval_name(keyval));
      }
   }

   return gboolean(handled);
}

void
on_python_scripting_entry_key_released(GtkEventControllerKey *controller,
                                       guint                  keyval,
                                       guint                  keycode,
                                       guint                  modifiers,
                                       GtkButton             *button) {

   graphics_info_t g;
   std::cout << "on_python_scripting_entry_key_released() keyval: " << keyval << " keycode: " << keycode << std::endl;

}


// 20230516-PE trying to add back the python completion and history that was in
// gtk3 coot into gtk4 coot.
//
// 20230516-PE I am, for the moment, not adding the header coot-setup-python.hh here because
// it doesn't include gtk stuff (for now).
void add_python_scripting_entry_completion(GtkWidget *entry);

void on_python_scripting_entry_activated(GtkEntry* entry, gpointer user_data) {

   const char *entry_txt = gtk_editable_get_text(GTK_EDITABLE(entry));
   g_info("Running python command: '%s'",entry_txt);
   PyRun_SimpleString(entry_txt);

   // add a copy of the text to history
   graphics_info_t::command_history.add_to_history(std::string(entry_txt));
   // clear the entry
   gtk_editable_set_text(GTK_EDITABLE(entry), "");
}

void setup_python_scripting_entry() {

   GtkWidget *entry = widget_from_builder("python_scripting_entry");
   if(entry == NULL) {
      g_error("'python_scripting_entry' from builder is NULL");
      return;
   }
   GtkEventController *key_controller_entry = gtk_event_controller_key_new();

   // for 'Up' and 'Down' keys, i.e. history lookup
   // and for 'Esc' key to hide the revealer

   g_signal_connect(key_controller_entry, "key-pressed",  G_CALLBACK(on_python_scripting_entry_key_pressed),  entry);
   // g_signal_connect(key_controller_entry, "key-released", G_CALLBACK(on_python_scripting_entry_key_released), entry);

   gtk_widget_add_controller(entry, key_controller_entry);

   // for executing Python commands
   g_signal_connect(entry, "activate", G_CALLBACK(on_python_scripting_entry_activated), entry);

   // PE adds history and completions (in coot-setup-python.cc)
   add_python_scripting_entry_completion(entry);
}

void setup_gui_components() {

   g_info("Initializing UI components...");
   setup_menubuttons();
   setup_validation_graph_dialog();
   setup_ramachandran_plot_chooser_dialog();
   setup_get_monomer();
   setup_accession_code_frame();
   setup_python_scripting_entry();
   attach_css_style_class_to_overlays();
   g_info("Done initializing UI components.");
}
