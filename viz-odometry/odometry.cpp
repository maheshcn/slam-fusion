#include "odometry.h"


Odometry::Odometry(QObject *parent) :
    QObject(parent)
{
    pWriter.openFile("/home/mahesh/out/predicted.csv"," ",1);
    std::vector<string> pHead;
   // pHead.push_back("x");
   // pHead.push_back("y");
   // pHead.push_back("z");
    pWriter.writeHead(pHead);


}

void Odometry::findOdometry()
{

    std::string dir = data_set_path;

      // set most important visual odometry parameters
      // for a full parameter list, look at: viso_stereo.h
      VisualOdometryStereo::parameters param;

      // calibration parameters for sequence 2010_03_09_drive_0019
      param.calib.f  = 645.24; // focal length in pixels
      param.calib.cu = 635.96; // principal point (u-coordinate) in pixels
      param.calib.cv = 194.13; // principal point (v-coordinate) in pixels
      param.base     = 0.5707; // baseline in meters

      // init visual odometry
      VisualOdometryStereo viso(param);

      // current pose (this matrix transforms a point from the current
      // frame's camera coordinates to the first frame's camera coordinates)
     pose = Matrix::eye(4);

      // loop through all frames i=0:372
      for (int32_t i=0; i<1424; i++) {

        // input file names
        char base_name[256]; sprintf(base_name,"%06d.png",i);
        std::string left_img_file_name  = dir + "/I1_" + base_name;
        std::string right_img_file_name = dir + "/I2_" + base_name;

        // catch image read/write errors here
        try {

          // load left and right input image
          leftImage=QImage(QString::fromUtf8(left_img_file_name.data(),left_img_file_name.size() ));
          rightImage=QImage(QString::fromUtf8(right_img_file_name.data(),right_img_file_name.size() ));
          png::image< png::gray_pixel > left_img(left_img_file_name);
          png::image< png::gray_pixel > right_img(right_img_file_name);

          // image dimensions
          int32_t width  = left_img.get_width();
          int32_t height = left_img.get_height();

          // convert input images to uint8_t buffer
          uint8_t* left_img_data  = (uint8_t*)malloc(width*height*sizeof(uint8_t));
          uint8_t* right_img_data = (uint8_t*)malloc(width*height*sizeof(uint8_t));
          int32_t k=0;
          for (int32_t v=0; v<height; v++) {
            for (int32_t u=0; u<width; u++) {
              //left_img_data[k]  = left_img.get_pixel(u,v);
              //right_img_data[k] = right_img.get_pixel(u,v);
                QRgb leftPix=leftImage.pixel(u,v);
                left_img_data[k]  = qGray(leftPix);
                QRgb rightPix=rightImage.pixel(u,v);
                right_img_data[k]  = qGray(rightPix);

                //right_img_data[k] = right_img.get_pixel(u,v);

              k++;
            }
          }


          // status
         // std::cout << "Processing: Frame: " << i;

          // compute visual odometry
          int32_t dims[] = {width,height,width};
          if (viso.process(left_img_data,right_img_data,dims)) {

            // on success, update current pose
            pose = pose * Matrix::inv(viso.getMotion());

            // output some statistics
            double num_matches = viso.getNumberOfMatches();
            double num_inliers = viso.getNumberOfInliers();
            //std::cout << ", Matches: " << num_matches;
            //std::cout << ", Inliers: " << 100.0*num_inliers/num_matches << " %" << ", Current pose: " << std::endl;
            //std::cout << pose << std::endl << std::endl;

          } else {
            std::cout << " ... failed!" << std::endl;
          }
          std::vector<FLOAT> pos_data;

          pos_data.push_back(pose.val[0][0]);
          pos_data.push_back(pose.val[0][1]);
          pos_data.push_back(pose.val[0][2]);
          pos_data.push_back(pose.val[0][3]);
          pos_data.push_back(pose.val[1][0]);
          pos_data.push_back(pose.val[1][1]);
          pos_data.push_back(pose.val[1][2]);
          pos_data.push_back(pose.val[1][3]);
          pos_data.push_back(pose.val[2][0]);
          pos_data.push_back(pose.val[2][1]);
          pos_data.push_back(pose.val[2][2]);
          pos_data.push_back(pose.val[2][3]);
          pWriter.writeLine(pos_data);
          // release uint8_t buffers
          free(left_img_data);
          free(right_img_data);

        // catch image read errors here
        } catch (...) {
          std::cerr << "ERROR: Couldn't read input files!" << std::endl;
          return ;
        }
      }
    pWriter.closeFile();

}

QImage  *Odometry::getOrgLeftImage()
{


    return &(leftImage);
}

Matrix * Odometry::getPosition()
{
    return &pose;
}

void Odometry::setDataPath(std::string path)
{
    data_set_path=path;
}
