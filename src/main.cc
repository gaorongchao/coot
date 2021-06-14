/* src/main.cc
 *
 * Copyright 2001, 2002, 2003, 2004, 2005, 2006, 2007 by The University of York
 * Copyright 2007, 2009, 2011, 2012 by The University of Oxford
 * Copyright 2014, 2015 by Medical Research Council
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA
 */

#include "coot-setup-python.hh"
#include "python-classes.hh"

#include "compat/coot-sysdep.h"

/*
 * Initial main.c file generated by Glade. Edit as required.
 * Glade will not overwrite this file.
 */

#include <sys/time.h>
#include <string.h> // strcmp

#include <iostream>

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif


// We are not using NLS yet.
// #ifndef WINDOWS_MINGW
// #define ENABLE_NLS
// #endif
// #ifdef DATADIR
// #endif // DATADIR

#include <gtk/gtk.h>

// #include <GL/glut.h> // for glutInit()

// #include "lbg/lbg.hh"

#include "interface.h"
#ifndef HAVE_SUPPORT_H
#define HAVE_SUPPORT_H
#include "support.h"
#endif /* HAVE_SUPPORT_H */


#include <sys/types.h> // for stating
#include <sys/stat.h>

#ifndef _MSC_VER
#include <unistd.h>
#else
#define PKGDATADIR "C:/coot/share"
#endif

#include "globjects.h"

#include <vector>
#include <string>

#include <mmdb2/mmdb_manager.h>
#include "coords/mmdb-extras.h"
#include "coords/mmdb.h"
#include "coords/mmdb-crystal.h"

#include "clipper/core/test_core.h"
#include "clipper/contrib/test_contrib.h"

#include "coords/Cartesian.h"
#include "coords/Bond_lines.h"

#include "command-line.hh"

#include "graphics-info.h"
// Including python needs to come after graphics-info.h, because
// something in Python.h (2.4 - chihiro) is redefining FF1 (in
// ssm_superpose.h) to be 0x00004000 (Grrr).
// BL says:: and (2.3 - dewinter), i.e. is a Mac - Python issue
// since the follwing two include python graphics-info.h is moved up
//
#if defined (WINDOWS_MINGW)
#ifdef DATADIR
#undef DATADIR
#endif // DATADIR
#endif
#include "compat/sleep-fixups.h"

#include "c-interface.h"
#include "c-interface-gtk-widgets.h"
#include "cc-interface.hh"
#include "c-interface-preferences.h"

// #include "coot-surface/rgbreps.h"

#include "coot-database.hh"

#include <glob.h>

#ifdef USE_GUILE
#include <libguile.h>
#endif


// #include "c-inner-main.h"
#include "coot-glue.hh"

#include "rotate-translate-modes.hh"

#include "change-dir.hh"
#include "curlew.hh"

void show_citation_request();
void load_gtk_resources();
void setup_splash_screen();
void desensitive_scripting_menu_item_maybe(GtkWidget *window);
int setup_screen_size_settings();
void setup_application_icon(GtkWindow *window);
void setup_symm_lib();
void check_reference_structures_dir();
void create_rot_trans_menutoolbutton_menu(GtkWidget *window1);
#include "boot-python.hh"

#ifdef USE_MYSQL_DATABASE
#include "mysql/mysql.h"
int setup_database();
#endif

#include "testing.hh" // for test_internal();

#include "scm-boot-guile.hh"

#include "widget-headers.hh" // put these somewhere else? better name? -------- GTK-FIME
#include "sound.hh"

#include "draw.hh" // for test_gtk3_adjustment_changed() - maybe that should go elsewhere?
#include "draw-2.hh"

void
windows_set_error_mode() {

#ifdef WINDOWS_MINGW
      // in Windows we don't want a crash dialog if no-graphics
      SetErrorMode(SetErrorMode(SEM_NOGPFAULTERRORBOX) | SEM_NOGPFAULTERRORBOX);
#endif // MINGW
}

GtkWidget *do_splash_screen(const command_line_data &cld) {

   GtkWidget *splash_screen = 0;
   setup_splash_screen();

   if (cld.use_splash_screen) {
      if (graphics_info_t::use_graphics_interface_flag) {
         std::string f = cld.alternate_splash_screen_file_name;
         if (f.empty()) {
            splash_screen = create_splash_screen_window();
         } else {
            splash_screen = create_splash_screen_window_for_file(f.c_str());
         }
         if (splash_screen) {
            gtk_widget_show(splash_screen);
         } else{
            std::cout << "ERROR:: in main() splash is null " << std::endl;
         }

         while(gtk_main_iteration() == FALSE);
         while (gtk_events_pending()) {
            usleep(3000);
            gtk_main_iteration();
         }
      }
   }
   return splash_screen;
}

void do_main_window(const command_line_data &cld) {

   GtkWidget *window1 = create_window1();

   graphics_info_t::set_main_window(window1);

   if (true) {
      std::string version_string = VERSION;
      std::string main_title = "Coot " + version_string;
#ifdef MAKE_ENHANCED_LIGAND_TOOLS
      main_title += " EL";
#endif

#ifdef COOT_MAIN_TITLE_EXTRA
      main_title += COOT_MAIN_TITLE_EXTRA;
#else
      // if this is a pre-release, stick in the revision number too
      if (version_string.find("-pre") != std::string::npos) {
	 main_title += " (revision count ";
	 main_title += coot::util::int_to_string(git_revision_count());
	 main_title += ")";
      }
#endif

#ifdef WINDOWS_MINGW
      main_title = "Win" + main_title;
#endif

      GtkWidget *model_toolbar = lookup_widget(window1, "model_toolbar");
      gtk_widget_show(model_toolbar);

      gtk_window_set_title(GTK_WINDOW (window1), main_title.c_str());
      GtkWidget *vbox = lookup_widget(window1, "main_window_vbox");
      // make this a grid, so that we can have 2x3 (say) graphics contexts
      GtkWidget *graphics_hbox = lookup_widget(window1, "main_window_graphics_hbox");

      GtkWidget *glarea = create_and_pack_gtkglarea(graphics_hbox, false);
      my_glarea_add_signals_and_events(glarea);
      graphics_info_t::glareas.push_back(glarea); // have I done this elsewhere?
      
      if (true) {
	 // application icon:
	 setup_application_icon(GTK_WINDOW(window1));
	 // adjust screen size settings
	 int small_screen = setup_screen_size_settings();

         // this needs to be fixed. I don't want to touch cld in this function.
         // Perhaps this function should return a flag for the small screen state?
         // or set something in graphics_info_t?
         //
	 // if (!cld.small_screen_display)
         //    cld.small_screen_display = small_screen;

	 gtk_widget_show(glarea);

         // other. Surely this never happens?
	 if (graphics_info_t::glareas.size() > 1)
	    gtk_widget_show(graphics_info_t::glareas[1]);

	 // and setup (store) the status bar
	 GtkWidget *sb = lookup_widget(window1, "main_window_statusbar");
	 graphics_info_t::statusbar = sb;
	 graphics_info_t::statusbar_context_id =
	    gtk_statusbar_get_context_id(GTK_STATUSBAR(sb), "picked atom info");

	 gtk_widget_show (window1);
	 create_rot_trans_menutoolbutton_menu(window1);

	 // We need to somehow connect the submenu to the menu's (which are
	 // accessible via window1)
	 //
	 create_initial_map_color_submenu(window1);
	 create_initial_ramachandran_mol_submenu(window1);
	 create_initial_sequence_view_mol_submenu(window1);

	 // old style non-generic functions
	 //      create_initial_validation_graph_b_factor_submenu(window1);
	 //      create_initial_validation_graph_geometry_submenu(window1);
	 //      create_initial_validation_graph_omega_submenu(window1);

	 // OK, these things work thusly:
	 //
	 // probe_clashes1 is the name of the menu_item set/created in
	 // by glade and is in mapview.glade.
	 //
	 // probe_submenu is something I make up. It must be the same
	 // here and in c-interface-validate.cc's
	 // add_on_validation_graph_mol_options()
	 //
	 // attach a function to the menu item activate function
	 // created by glade in callbacks.c
	 // (e.g. on_probe_clashes1_activate).  The name that is used
	 // there to look up the menu is as above (e.g. probe_clashes1).
	 //
	 // The type defined there is that checked in
	 // c-interface-validate.cc's
	 // add_on_validation_graph_mol_options()


	 create_initial_validation_graph_submenu_generic(window1 , "peptide_omega_analysis1", "omega_submenu");
	 create_initial_validation_graph_submenu_generic(window1 , "geometry_analysis1", "geometry_submenu");
	 create_initial_validation_graph_submenu_generic(window1 , "temp_fact_variance_analysis1",
							 "temp_factor_variance_submenu");
////ADDITION B FACTOR GRAPH
	create_initial_validation_graph_submenu_generic(window1 , "temp_fact_analysis1", "temp_factor_submenu");
//// END ADD B FACTOR GRAPH
	 create_initial_validation_graph_submenu_generic(window1 , "rotamer_analysis1", "rotamer_submenu");
	 create_initial_validation_graph_submenu_generic(window1 , "density_fit_analysis1", "density_fit_submenu");
	 create_initial_validation_graph_submenu_generic(window1 , "probe_clashes1", "probe_submenu");
	 create_initial_validation_graph_submenu_generic(window1 , "gln_and_asn_b_factor_outliers1",
							 "gln_and_asn_b_factor_outliers_submenu");
	 create_initial_validation_graph_submenu_generic(window1 , "ncs_differences1", "ncs_diffs_submenu");

         // OK, now we can import the python coot_gui and extensions
         import_python_module("coot_gui",   0);
         import_python_module("populate_python_menus", 0);

      } else {
	 std::cout << "CATASTROPHIC ERROR:: failed to create Gtk GL widget"
		   << "  (Check that your X11 server is working and has (at least)"
		   << "  \"Thousands of Colors\" and supports GLX.)" << std::endl;
      }
   }
}

int
do_self_tests() {

   std::cout << "INFO:: Running internal self tests" << std::endl;
   // return true on success
   clipper::Test_core test_core;       bool result_core    = test_core();
   clipper::Test_contrib test_contrib; bool result_contrib = test_contrib();
   std::cout<<" INFO:: Test Clipper core   : "<<(result_core   ?"OK":"FAIL")<<std::endl;
   std::cout<<" INFO:: Test Clipper contrib: "<<(result_contrib?"OK":"FAIL")<<std::endl;
   // return 1 on success
   int gis = test_internal();
   int shell_exit_code = 1;
   if (result_core)
      if (result_contrib)
         if (gis == 1)
            shell_exit_code = 0;
   return shell_exit_code;

}

void on_glarea_realize(GtkGLArea *glarea);

void init_from_gtkbuilder() {

   std::string glade_file_full = "a6.glade"; // do this properly at some stage
   GtkBuilder *builder = gtk_builder_new();

   guint add_from_file_status = gtk_builder_add_from_file(builder, glade_file_full.c_str(), NULL);
   std::cout << "add_from_file_status " << add_from_file_status << std::endl;

   GtkWidget *graphics_hbox = GTK_WIDGET(gtk_builder_get_object(builder, "main_window_graphics_hbox"));

   if (graphics_hbox) {

      graphics_info_t::set_gtkbuilder(builder); // store for future widget queries

      GtkWidget *main_window = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));

      GtkWidget *sb = GTK_WIDGET(gtk_builder_get_object(builder, "main_window_statusbar"));

      std::cout << "debug:: main_window "   << main_window << std::endl;
      std::cout << "debug:: graphics_hbox " << graphics_hbox << std::endl;
      std::cout << "debug:: statusbar "     << sb << std::endl;

      if (main_window)
         graphics_info_t::set_main_window(main_window);

      graphics_info_t::statusbar = sb;
      GtkWidget *glarea = create_and_pack_gtkglarea(graphics_hbox, true);
      if (glarea) {
         graphics_info_t::glareas.push_back(glarea);

         // gtk_gl_area_make_current(GTK_GL_AREA(glarea));
         GError *err = gtk_gl_area_get_error(GTK_GL_AREA(glarea));
         if (err)
            std::cout << "ERROR in init()" << err << std::endl;
         
         gtk_builder_connect_signals(builder, main_window);
         gtk_widget_show(main_window);

         if (false) {
            GtkWidget *w = gtk_label_new("Some Test Label");
            gtk_widget_show(w);
            gtk_box_pack_start(GTK_BOX(graphics_hbox), w, FALSE, FALSE, 2);
         }

      } else {
         std::cout << "init_main_window() glarea null" << std::endl;
      }
   } else {
      std::cout << "graphics_hbox was null" << std::endl;
   }
}


// This main is used for both python/guile useage and unscripted.
int
main (int argc, char *argv[]) {

   int shell_exit_code = 0;
   GtkWidget *window1 = NULL;

   graphics_info_t graphics_info;
   graphics_info.coot_is_a_python_module = false;

#ifdef ENABLE_NLS // not used currently in Gtk1. Gkt2, yes.
   //
   bindtextdomain (PACKAGE, PACKAGE_LOCALE_DIR);
   bind_textdomain_codeset (PACKAGE, "UTF-8");
   textdomain (PACKAGE);
#endif

#ifdef USE_LIBCURL
   curl_global_init(CURL_GLOBAL_NOTHING); // nothing extra (e.g. ssl or WIN32)
#endif

   command_line_data cld = parse_command_line(argc, argv);
   cld.handle_immediate_settings();

#ifdef USE_PYTHON

   // When using the builder, we need to setup python after setting up gtk (and storing the widgets)
   if (! cld.use_gtkbuilder) {
      setup_python(argc, argv);
      // setup_python_classes();
   }

#endif

#ifdef WITH_SOUND
   // test_sound(argc, argv);
#endif // WITH_SOUND

   if (cld.run_internal_tests_and_exit)
      shell_exit_code = do_self_tests();

   if (graphics_info_t::show_citation_notice == 1)
      show_citation_request();

   if (graphics_info_t::use_graphics_interface_flag) {
      // load_gtk_resources();
      gtk_init (&argc, &argv);
      // activate to force icons in menus; cannot get it to work with
      // cootrc. Bug?
      // seems to be neccessary to make sure the type is realized
      // and we use newer g_object_set instead of deprecated (gtk3)
      // gtk_settings_set_long_property

      // not sure what this does, but it's deprecated now
      // g_type_class_unref (g_type_class_ref (GTK_TYPE_IMAGE_MENU_ITEM));

      g_object_set(gtk_settings_get_default(), "gtk-menu-images", TRUE, NULL);
      g_object_set(gtk_settings_get_default(), "gtk-application-prefer-dark-theme", TRUE, NULL);

   } else {

      windows_set_error_mode();

   }

   GtkWidget *splash = do_splash_screen(cld);

   setup_symm_lib();
   check_reference_structures_dir();

   graphics_info.init();

   if (graphics_info_t::use_graphics_interface_flag) {

      bool old_way_flag = true;
      if (cld.use_gtkbuilder)
         old_way_flag = false;

      if (! cld.use_gtkbuilder) {
         do_main_window(cld);
      }

      if (cld.use_gtkbuilder) {
         init_from_gtkbuilder();
         GtkWidget *glarea = graphics_info_t::glareas[0];
	 setup_application_icon(GTK_WINDOW(graphics_info_t::get_main_window()));

         gtk_widget_show(glarea);
         my_glarea_add_signals_and_events(glarea);
         std::cout << "............ done setup signals and events " << std::endl;
         on_glarea_realize(GTK_GL_AREA(glarea)); // hacketty hack. I don't know why realize is not called
                                                 // without this.
#ifdef USE_PYTHON
         // std::cout << "------------------------- calling setup_python" << std::endl;
         setup_python(argc, argv);
#endif
      }

   }

   // Mac users often start somewhere where they can't write files
   //
   change_directory_maybe();

#if !defined(USE_GUILE) && !defined(USE_PYTHON)
   handle_command_line_data(cld);  // and add a flag if listener
                                   // should be started.
#endif

   if (splash)
      gtk_widget_destroy(splash);

   // before we run the scripting, let's make default preferences
   make_preferences_internal_default();

   // this glade file is derived from an ancient coot. It doesn't have a
   // ligand builder menu item. Add this back when it does.
   //
   // add_ligand_builder_menu_item_maybe();             // FIXME

   // remove Curlew from the File menu with old compiler?
   remove_file_curlew_menu_item_maybe();

   // to start the graphics, we need to init glut and gtk with the
   // command line args.

   desensitive_scripting_menu_item_maybe(window1);

   // Hack this in to get Python scripts to work - not sure where the correct place to put this is.
   //
#ifdef USE_PYTHON
   handle_command_line_data(cld);
#endif

#ifdef USE_GUILE
   // *Don't* run script from here (before we start guile!)
#else
   //but if ware are using python only, python has been set up before now
   //
   // these scripts are stored by handle_command_line_data()
   run_command_line_scripts();
#endif


   // control goes into my_wrap_scm_boot_guile and doesn't return
#ifdef USE_GUILE
   // Must be the last thing in this function, code after it does not get
   // executed (if we are using guile)
   //
   std::cout << "------------------------------- into scm_boot_guile we go!" << std::endl;
   my_wrap_scm_boot_guile(argc, argv);
#endif


   // ==================== ususually not exectuted ========================

#if ! defined (USE_GUILE)
#ifdef USE_PYTHON

   if (graphics_info_t::use_graphics_interface_flag)
      gtk_main();
   else {
      start_command_line_python_maybe(true, argc, argv);
   }

#else
   // not python or guile
   if (graphics_info_t::use_graphics_interface_flag)
      gtk_main();
#endif // USE_PYTHON

#endif // ! USE_GUILE

   return shell_exit_code;
}

void desensitive_scripting_menu_item_maybe(GtkWidget *window1) {

   // Finally desensitize the missing scripting menu
   if (graphics_info_t::use_graphics_interface_flag) {
      GtkWidget *w;
#ifndef USE_GUILE
      // This lookup fails - I don't know why. Get rid of it for now - to remove startup message
      // w = lookup_widget(window1, "scripting_scheme1");
      // std::cout << "debug:: in desensitive_scripting_menu_item_maybe() w " << w << std::endl;
      // gtk_widget_set_sensitive(w, FALSE);
#endif
#ifndef USE_PYTHON
      w = lookup_widget(window1, "scripting_python1");
      gtk_widget_set_sensitive(w, FALSE);
#endif
   }
}


void load_gtk_resources() {

#if 0
   std::string gtkrcfile = PKGDATADIR;
   gtkrcfile += "/cootrc";

   // over-ridden by user?
   char *s = getenv("COOT_RESOURCES_FILE");
   if (s) {
      gtkrcfile = s;
   }

   std::cout << "Acquiring application resources from " << gtkrcfile << std::endl;
   gtk_rc_add_default_file(gtkrcfile.c_str()); // Deprecated - use GtkStyleContext
#endif
}


/*  ----------------------------------------------------------------------- */
/*            Amusing (possibly) little splash screen                       */
/*  ----------------------------------------------------------------------- */
void
setup_splash_screen() {

   // default location:
   std::string splash_screen_pixmap_dir = PKGDATADIR;
   splash_screen_pixmap_dir += "/";
   splash_screen_pixmap_dir += "pixmaps";

   // over-ridden by user?
   char *s = getenv("COOT_PIXMAPS_DIR");
   if (s) {
      splash_screen_pixmap_dir = s;
   }

   if (0)
      std::cout << "INFO:: splash_screen_pixmap_dir "
		<< splash_screen_pixmap_dir << std::endl;

   // now add splash_screen_pixmap_dir to the pixmaps_directories CList
   //
   add_pixmap_directory(splash_screen_pixmap_dir.c_str());

}



void
show_citation_request() {

   std::cout << "\n   If you have found this software to be useful, you are requested to cite:\n"
	     << "   Coot: model-building tools for molecular graphics" << std::endl;
   std::cout << "   Emsley P, Cowtan K" << std::endl;
   std::cout << "   ACTA CRYSTALLOGRAPHICA SECTION D-BIOLOGICAL CRYSTALLOGRAPHY\n";
   std::cout << "   60: 2126-2132 Part 12 Sp. Iss. 1 DEC 2004\n\n";

   std::cout << "   The reference for the REFMAC5 Dictionary is:\n";
   std::cout << "   REFMAC5 dictionary: organization of prior chemical knowledge and\n"
	     << "   guidelines for its use" << std::endl;
   std::cout << "   Vagin AA, Steiner RA, Lebedev AA, Potterton L, McNicholas S,\n"
	     << "   Long F, Murshudov GN" << std::endl;
   std::cout << "   ACTA CRYSTALLOGRAPHICA SECTION D-BIOLOGICAL CRYSTALLOGRAPHY " << std::endl;
   std::cout << "   60: 2184-2195 Part 12 Sp. Iss. 1 DEC 2004" << std::endl;

#ifdef HAVE_SSMLIB
    std::cout << "\n   If using \"SSM Superposition\", please cite:\n";

    std::cout << "   Secondary-structure matching (SSM), a new tool for fast\n"
	      << "   protein structure alignment in three dimensions" << std::endl;
    std::cout << "   Krissinel E, Henrick K" << std::endl;
    std::cout << "   ACTA CRYSTALLOGRAPHICA SECTION D-BIOLOGICAL CRYSTALLOGRAPHY" << std::endl;
    std::cout << "   60: 2256-2268 Part 12 Sp. Iss. 1 DEC 2004\n" << std::endl;
#endif // HAVE_SSMLIB

}


void
menutoolbutton_rot_trans_activated(GtkWidget *item, GtkPositionType pos) {

   // std::cout << "changing to zone type" << pos << std::endl;
   set_rot_trans_object_type(pos);
   do_rot_trans_setup(1);
   if (graphics_info_t::model_fit_refine_dialog) {
     update_model_fit_refine_dialog_menu(graphics_info_t::model_fit_refine_dialog);
   }
}

void create_rot_trans_menutoolbutton_menu(GtkWidget *window1) {

   //
   GtkWidget *menu_tool_button = lookup_widget(window1, "model_toolbar_rot_trans_toolbutton");

   if (menu_tool_button) {
      GtkWidget *menu = gtk_menu_new();
      GtkWidget *menu_item;
      GSList *group = NULL;

      menu_item = gtk_radio_menu_item_new_with_label(group, "By Residue Range...");
      group = gtk_radio_menu_item_get_group(GTK_RADIO_MENU_ITEM(menu_item));
      gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);
      gtk_widget_show(menu_item);
      g_signal_connect(G_OBJECT(menu_item), "activate",
	              (GCallback) (menutoolbutton_rot_trans_activated),
		GINT_TO_POINTER(ROT_TRANS_TYPE_ZONE));
      /* activate the first item */
      gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(menu_item), TRUE);

      menu_item = gtk_radio_menu_item_new_with_label(group, "By Chain...");
      group = gtk_radio_menu_item_get_group(GTK_RADIO_MENU_ITEM(menu_item));
      gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);
      gtk_widget_show(menu_item);
      g_signal_connect(G_OBJECT(menu_item), "activate",
			 (GCallback) (menutoolbutton_rot_trans_activated),
			 GINT_TO_POINTER(ROT_TRANS_TYPE_CHAIN));

      menu_item = gtk_radio_menu_item_new_with_label(group, "By Molecule...");
      group = gtk_radio_menu_item_get_group(GTK_RADIO_MENU_ITEM(menu_item));
      gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);
      gtk_widget_show(menu_item);
      g_signal_connect(G_OBJECT(menu_item), "activate",
			 (GCallback) (menutoolbutton_rot_trans_activated),
			 GINT_TO_POINTER(ROT_TRANS_TYPE_MOLECULE));

      gtk_menu_tool_button_set_menu(GTK_MENU_TOOL_BUTTON(menu_tool_button), menu);
   }
}
