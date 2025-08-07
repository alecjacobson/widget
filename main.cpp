#include <igl/opengl/glfw/Viewer.h>
#include <igl/readOBJ.h>
#include <igl/unproject_onto_mesh.h>
#include <igl/unproject.h>
#include <igl/slice.h>

int main(int argc, char *argv[])
{
  Eigen::MatrixXd V;
  Eigen::MatrixXi F;
  Eigen::VectorXi I;
  Eigen::MatrixXd M;
  {
    Eigen::MatrixXi FTC,FN;
    Eigen::MatrixXd N;
    igl::readOBJ("../widget.obj",V,M,N,F,FTC,FN);
    I = FTC.col(0);
  }
  Eigen::MatrixXd FC;
  igl::slice(M,I,1,FC);


  // Plot the mesh
  igl::opengl::glfw::Viewer vr;
  vr.data().set_mesh(V, F);
  vr.data().set_colors(FC);
  vr.data().show_lines = false;

  int widget_down = -1;
  double widget_down_z;
  vr.callback_mouse_down = [&](igl::opengl::glfw::Viewer &,int,int)
  {
    int fid;
    Eigen::Vector3f bc;
    // Cast a ray in the view direction starting from the mouse position
    double x = vr.current_mouse_x;
    double y = vr.core().viewport(3) - vr.current_mouse_y;
    if(igl::unproject_onto_mesh(Eigen::Vector2f(x,y), vr.core().view,
      vr.core().proj, vr.core().viewport, V, F, fid, bc))
    {
      widget_down_z =  V(F(fid,0),2)*bc(0)+V(F(fid,1),2)*bc(1)+V(F(fid,2),2)*bc(2);
      widget_down = I(fid);
      // update color
      Eigen::MatrixXd MM = M;
      MM.row(I(fid)).setConstant(1);
      Eigen::MatrixXd FC;
      igl::slice(MM,I,1,FC);
      vr.data().set_colors(FC);
      return true;
    }
    return false;
  };
  vr.callback_mouse_move = [&](igl::opengl::glfw::Viewer &,int,int)
  {
    if(widget_down<0){return false;}
    switch(widget_down)
    {
      case 0:
      {
        const Eigen::Vector3f t = 
          igl::unproject(
            Eigen::Vector3f(
              vr.down_mouse_x,
              vr.down_mouse_y,widget_down_z),vr.core().view,vr.core().proj,vr.core().viewport);
        break;
      }
    }
    return true;
  };
  vr.callback_mouse_up = [&](igl::opengl::glfw::Viewer &,int,int)
  {
    widget_down = -1;
    // update color
    Eigen::MatrixXd FC;
    igl::slice(M,I,1,FC);
    vr.data().set_colors(FC);
    return false;
  };

  vr.launch();
}
