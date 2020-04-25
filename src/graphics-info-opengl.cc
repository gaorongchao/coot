
#ifdef USE_PYTHON
#include <Python.h>
#endif

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext.hpp>
#include <glm/gtx/string_cast.hpp>
#include "graphics-info.h"

void
graphics_info_t::init_shaders() {

   std::cout << "--------------------- init_shaders() --------" << std::endl;
   graphics_info_t::shader_for_maps.init("map.shader", Shader::Entity_t::MAP);
   graphics_info_t::shader_for_models.init("model.shader", Shader::Entity_t::MODEL);
   graphics_info_t::shader_for_central_cube.init("central-cube.shader", Shader::Entity_t::INFRASTRUCTURE);
   graphics_info_t::shader_for_origin_cube.init("central-cube.shader", Shader::Entity_t::INFRASTRUCTURE);
   graphics_info_t::shader_for_hud_text.init("hud-text.shader", Shader::Entity_t::HUD_TEXT);
   graphics_info_t::shader_for_atom_labels.init("atom-label.shader", Shader::Entity_t::TEXT_3D);

   // we use the above to make an image/texture in the framebuffer and use then
   // shader_for_screen to convert that framebuffer to the screen buffer.
   graphics_info_t::shader_for_screen.init("screen.shader", Shader::Entity_t::SCREEN);
   graphics_info_t::shader_for_blur.init("blur.shader", Shader::Entity_t::SCREEN);



}

int
graphics_info_t::blob_under_pointer_to_screen_centre() {

   // a quick slide would be better than a jump

   graphics_info_t g; // needed?
   int r = 0;
   if (use_graphics_interface_flag) {
      int imol_map = Imol_Refinement_Map();
      if (imol_map != -1) {
	 // OK we have a map to search.
	 // coot::Cartesian front = unproject(0.0);
	 // coot::Cartesian back  = unproject(1.0);
         // glm::vec4 glm_front = new_unproject(-0.3);
         // glm::vec4 glm_back  = new_unproject( 1.0);

         GtkAllocation allocation = graphics_info_t::get_glarea_allocation();
         int w = allocation.width;
         int h = allocation.height;

         glm::mat4 mvp = graphics_info_t::get_molecule_mvp(); // modeglml matrix includes orientation with the quaternion
         glm::mat4 vp_inv = glm::inverse(mvp);

         float mouseX_2 = g.mouse_current_x  / (w * 0.5f) - 1.0f;
         float mouseY_2 = g.mouse_current_y  / (h * 0.5f) - 1.0f;
         // I revered the sign here - it does the right thing now.
         glm::vec4 screenPos_1 = glm::vec4(mouseX_2, -mouseY_2, -1.0f, 1.0f);
         glm::vec4 screenPos_2 = glm::vec4(mouseX_2, -mouseY_2,  1.0f, 1.0f);
         glm::vec4 worldPos_1 = vp_inv * screenPos_1;
         glm::vec4 worldPos_2 = vp_inv * screenPos_2;

         coot::Cartesian front(worldPos_1.x, worldPos_1.y, worldPos_1.z);
	 coot::Cartesian  back(worldPos_2.x, worldPos_2.y, worldPos_2.z);
	 clipper::Coord_orth p1(front.x(), front.y(), front.z());
	 clipper::Coord_orth p2( back.x(),  back.y(),  back.z());
         if (false) {
            std::cout << "blob_under_pointer_to_screen_centre() " << glm::to_string(screenPos_1) << " "
                      << glm::to_string(screenPos_2) << std::endl;
            std::cout << "blob_under_pointer_to_screen_centre() " << front << " " << back << std::endl;
            // std::cout << "blob_under_pointer_to_screen_centre() " << p1.format() << " "
            // << p2.format() << std::endl;
         }
         coot::Cartesian rc = g.RotationCentre();

	 try {
	    clipper::Coord_orth blob =
	       molecules[imol_refinement_map].find_peak_along_line_favour_front(p1, p2);
	    coot::Cartesian cc(blob.x(), blob.y(), blob.z());
            // coot::Cartesian cc = front.mid_point(back);
            coot::Cartesian delta = rc - cc;
            // std::cout << "Delta: " << delta << std::endl;
	    g.setRotationCentre(cc);
	    for(int ii=0; ii<n_molecules(); ii++) {
	       molecules[ii].update_map();
	       molecules[ii].update_symmetry();
	    }
	    g.make_pointer_distance_objects();
	    graphics_draw();
	 }
	 catch (const std::runtime_error &mess) {
	    std::cout << mess.what() << std::endl;
	 }
      } else {
	 std::string s = "WARNING:: Refinement map not selected - no action";
	 std::cout << s << std::endl;
	 // add_status_bar_text(s.c_str());
	 info_dialog(s.c_str());
      }
   }
   return r;
}


void
graphics_info_t::set_clipping_front(float v) {

   float clipping_max = 15.0; // was 10
   graphics_info_t::clipping_front = v;
   if (graphics_info_t::clipping_front > clipping_max)
      graphics_info_t::clipping_front = clipping_max;
   graphics_draw();
}


void
graphics_info_t::set_clipping_back(float v) {

   float clipping_max = 15.0;
   graphics_info_t::clipping_back = v;
   if (graphics_info_t::clipping_back > clipping_max)
      graphics_info_t::clipping_back = clipping_max;
   graphics_draw();
}


void
graphics_info_t::adjust_clipping(float d) {

   if (! graphics_info_t::perspective_projection_flag) {

      clipping_front = clipping_front * (1.0 + d);
      clipping_back  = clipping_back  * (1.0 + d);

   } else {

      // --- perspective ---

      glm::vec3 rc = get_rotation_centre();

      double l = glm::distance(eye_position, rc);
      double zf = screen_z_far_perspective;
      double zn = screen_z_near_perspective;

      // we should (and now do) concern ourselves with the distance to
      // the rotation centre so that the clipping planes are not
      // changed so that rotation centre is clipped.

      if (d < 0) {

         // close down (narrow) - is this correct? :-)

         screen_z_near_perspective = l - (l-zn) * 0.9905;
         screen_z_far_perspective  = l + (zf-l) * 0.95;

      } else {

         // expand

         screen_z_far_perspective  = l + (zf-l) * 1.05;
         screen_z_near_perspective = l - (l-zn) * 1.005;

      }


      float screen_z_near_perspective_limit = l * 0.99;
      float screen_z_far_perspective_limit  = l * 1.01;
      if (screen_z_near_perspective > screen_z_near_perspective_limit)
         screen_z_near_perspective = screen_z_near_perspective_limit;
      if (screen_z_far_perspective < screen_z_far_perspective_limit)
         screen_z_far_perspective = screen_z_far_perspective_limit;

      std::cout << "debug l " << l << " near and far: pre: " << zn << " " << zf
                << "    post " << screen_z_near_perspective << " "
                << screen_z_far_perspective << std::endl;
   }
}

//static
void
graphics_info_t::update_view_quaternion(int area_width, int area_height) {

   graphics_info_t g;
   float tbs = g.get_trackball_size();

   if (perspective_projection_flag)
      tbs = 1.0;

   glm::quat tb_quat =
      g.trackball_to_quaternion((2.0*g.GetMouseBeginX() - area_width)/area_width,
                                (area_height - 2.0*g.GetMouseBeginY())/area_height,
                                (2.0*g.mouse_current_x - area_width)/area_width,
                                (area_height - 2.0*g.mouse_current_y)/area_height,
                                tbs);

   if (! graphics_info_t::perspective_projection_flag) {

      tb_quat = glm::conjugate(tb_quat); // hooray, no more "backwards" mouse motion
      glm::quat product = tb_quat * glm_quat;
      glm_quat = glm::normalize(product);

   } else {

      // move the eye according to tb_quat

      // glm::quat product = tb_quat * glm_quat;
      // glm_quat = glm::normalize(product);

      if (false)
         std::cout << "debug:: glm_quat quaternion " << glm::to_string(glm_quat) << std::endl;

      float delta_x = mouse_current_x - g.GetMouseBeginX();
      float delta_y = mouse_current_y - g.GetMouseBeginY();

      glm::quat quat_new_1 = glm::rotate(glm_quat, delta_x, glm::vec3(1,0,0));

      glm::vec3 rc = get_rotation_centre();
      glm::vec3 eye_from_rotation_centre = eye_position - rc;
      glm::vec4 ep4(eye_from_rotation_centre, 1.0);
      glm::mat4 matrix = glm::inverse(glm::toMat4(tb_quat));
      glm::vec4 new_rel_eye_pos = matrix * ep4;
      eye_position = glm::vec3(new_rel_eye_pos) + rc;
   }
}
