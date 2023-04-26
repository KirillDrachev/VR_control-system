//#define COMPILE_VR
//#define COMPILE_ZED


#include <glad/glad.h>    //для opengl

#ifdef COMPILE_VR
#include <openvr/openvr.h>  //openvr
#include <openvr/openvr_capi.h>  //openvr
#pragma comment(lib,"openvr_api64.lib")
#include <openvr/openvr_driver.h>
#endif

#include <SFML/Graphics.hpp>   //window and opengl context
#include <SFML/Window.hpp>   //window and opengl context
//#include <SFML/OpenGL.hpp>
#include <iostream>   //debug

#ifdef COMPILE_ZED
#include <ZED/Camera.hpp>   //Zed cam
#endif

#include <Shader_program.h>  //shader loader and compiler class
#include <display_plane.h>   //display class
#include <glm/glm.hpp>       //math
#include <glm/gtc/matrix_transform.hpp>  //math
#include <glm/gtc/type_ptr.hpp>    //math



#ifdef COMPILE_VR
//openvr
vr::HmdError eError = vr::VRInitError_None;
vr::IVRSystem * MyVR;
vr::HmdMatrix44_t VrProjMat;
struct HMDProperties_t {
    glm::vec3 PositionVec;
    glm::vec3 ForwardVec;
    glm::vec3 UpwardVec;
};
HMDProperties_t HMDProperties;
struct ControlleProperties {
    bool IsConnected;
    bool IsLeft;
    glm::vec3 PositionVec;
    glm::vec3 ForwardVec;
    glm::vec3 UpwardVec;
};
ControlleProperties LeftControllerProperties;
ControlleProperties RightControllerProperties;

vr::VRControllerState_t HMDState;
vr::VRControllerState_t LeftControllerState;
vr::VRControllerState_t RightControllerState;
#endif
//HMD resolution
unsigned int HMD_WIDTH = 1440 / 2;
unsigned int HMD_HEIGHT = 1700 / 2;
//Window resolution
unsigned int WINDOW_WIDTH = 1440 / 2;
unsigned int WINDOW_HEIGHT = 1700 / 2;
//Camera resolution
unsigned int CAM_WINDOW_WIDTH;
unsigned int CAM_WINDOW_HEIGHT;

#ifdef COMPILE_ZED
//Camera 
sl::Camera zed;
sl::InitParameters init_param;
sl::RuntimeParameters run_time_param;
sl::ERROR_CODE ZED_err;
sl::CameraInformation ZED_inf;
#endif
char* left_image_Ptr;
char* right_image_Ptr;
//SFML

sf::Time SPF;
//opengl
glm::mat4 ProjectionMat;
void* ProjectionMat_Ptr;
Shader_Program displayShPr;

#ifdef COMPILE_VR
bool VR_init() {
    eError = vr::VRInitError_None;
    MyVR = vr::VR_Init(&eError, vr::VRApplication_Scene, nullptr);
    std::cout << vr::VR_GetVRInitErrorAsEnglishDescription(eError) << std::endl;
    if (eError != vr::VRInitError_None) {
        return false;
    }
    vr::VRSystem()->GetRecommendedRenderTargetSize(&HMD_WIDTH, &HMD_HEIGHT);
    //HMD_WIDTH = HMD_WIDTH / 2;
    //HMD_HEIGHT = HMD_HEIGHT / 2;
    WINDOW_WIDTH = HMD_WIDTH /3;
    WINDOW_HEIGHT = HMD_HEIGHT / 3;
    std::cout <<"HMD render resolution: " <<HMD_WIDTH <<" x "<< HMD_HEIGHT << std::endl;
    //MyVR->ResetSeatedZeroPose();
    vr::VRCompositor()->SetTrackingSpace(vr::TrackingUniverseOrigin::TrackingUniverseSeated);
    VrProjMat = MyVR->GetProjectionMatrix(vr::Eye_Left, 0.1f, 2.0f);
    
    return true;
}
void UpdateDeviceStates() {
    for (vr::TrackedDeviceIndex_t Device = 0; Device < vr::k_unMaxTrackedDeviceCount; Device++){
        if (!MyVR->IsTrackedDeviceConnected(Device)) continue;
        vr::TrackedDevicePose_t pose;
        switch (vr::VRSystem()->GetTrackedDeviceClass(Device)) {
        case vr::ETrackedDeviceClass::TrackedDeviceClass_HMD:
            if(!vr::VRSystem()->GetControllerStateWithPose(vr::TrackingUniverseOrigin::TrackingUniverseSeated,
                Device, &HMDState,sizeof(HMDState) ,&pose)) continue;
            HMDProperties.PositionVec.x = pose.mDeviceToAbsoluteTracking.m[0][3];
            HMDProperties.PositionVec.y = pose.mDeviceToAbsoluteTracking.m[1][3];
            HMDProperties.PositionVec.z = pose.mDeviceToAbsoluteTracking.m[2][3];

            //std::cout << " HMD x: " << pose.mDeviceToAbsoluteTracking.m[0][3];
            //std::cout << " HMD y: " << pose.mDeviceToAbsoluteTracking.m[1][3];
            //std::cout << " HMD z: " << pose.mDeviceToAbsoluteTracking.m[2][3] << std::endl;

            HMDProperties.UpwardVec.x = pose.mDeviceToAbsoluteTracking.m[0][1];
            HMDProperties.UpwardVec.y = pose.mDeviceToAbsoluteTracking.m[1][1];
            HMDProperties.UpwardVec.z = pose.mDeviceToAbsoluteTracking.m[2][1];

            HMDProperties.ForwardVec.x = pose.mDeviceToAbsoluteTracking.m[0][2];
            HMDProperties.ForwardVec.y = pose.mDeviceToAbsoluteTracking.m[1][2];
            HMDProperties.ForwardVec.z = pose.mDeviceToAbsoluteTracking.m[2][2];

        case vr::ETrackedDeviceClass::TrackedDeviceClass_Controller:
            vr::ETrackedControllerRole role;
            role = vr::VRSystem()->GetControllerRoleForTrackedDeviceIndex(Device);
            if (role == vr::TrackedControllerRole_LeftHand) {
                if (!vr::VRSystem()->GetControllerStateWithPose(vr::TrackingUniverseOrigin::TrackingUniverseSeated,
                    Device, &LeftControllerState, sizeof(LeftControllerState), &pose)) continue;

                LeftControllerProperties.PositionVec.x = pose.mDeviceToAbsoluteTracking.m[0][3];
                LeftControllerProperties.PositionVec.y = pose.mDeviceToAbsoluteTracking.m[1][3];
                LeftControllerProperties.PositionVec.z = pose.mDeviceToAbsoluteTracking.m[2][3];

                /*std::cout << " x: " << pose.mDeviceToAbsoluteTracking.m[0][3];
                std::cout << " y: " << pose.mDeviceToAbsoluteTracking.m[1][3];
                std::cout << " z: " << pose.mDeviceToAbsoluteTracking.m[2][3] << std::endl;*/


                LeftControllerProperties.UpwardVec.x = pose.mDeviceToAbsoluteTracking.m[0][1];
                LeftControllerProperties.UpwardVec.y = pose.mDeviceToAbsoluteTracking.m[1][1];
                LeftControllerProperties.UpwardVec.z = pose.mDeviceToAbsoluteTracking.m[2][1];

                LeftControllerProperties.ForwardVec.x = pose.mDeviceToAbsoluteTracking.m[0][2];
                LeftControllerProperties.ForwardVec.y = pose.mDeviceToAbsoluteTracking.m[1][2];
                LeftControllerProperties.ForwardVec.z = pose.mDeviceToAbsoluteTracking.m[2][2];
            }
            else if (role == vr::TrackedControllerRole_RightHand) {
                if (!vr::VRSystem()->GetControllerStateWithPose(vr::TrackingUniverseOrigin::TrackingUniverseSeated,
                    Device, &RightControllerState, sizeof(RightControllerState), &pose)) continue;

                RightControllerProperties.PositionVec.x = pose.mDeviceToAbsoluteTracking.m[0][3];
                RightControllerProperties.PositionVec.y = pose.mDeviceToAbsoluteTracking.m[1][3];
                RightControllerProperties.PositionVec.z = pose.mDeviceToAbsoluteTracking.m[2][3];

                RightControllerProperties.UpwardVec.x = pose.mDeviceToAbsoluteTracking.m[0][1];
                RightControllerProperties.UpwardVec.y = pose.mDeviceToAbsoluteTracking.m[1][1];
                RightControllerProperties.UpwardVec.z = pose.mDeviceToAbsoluteTracking.m[2][1];

                RightControllerProperties.ForwardVec.x = pose.mDeviceToAbsoluteTracking.m[0][2];
                RightControllerProperties.ForwardVec.y = pose.mDeviceToAbsoluteTracking.m[1][2];
                RightControllerProperties.ForwardVec.z = pose.mDeviceToAbsoluteTracking.m[2][2];
            }
        }
    }
}

void TransformControllerPosToHeadPos() {
    LeftControllerProperties.PositionVec = LeftControllerProperties.PositionVec - HMDProperties.PositionVec;
    RightControllerProperties.PositionVec = RightControllerProperties.PositionVec - HMDProperties.PositionVec;
}
#endif

#ifdef COMPILE_ZED
bool ZED_init() {
    //ZED init
    init_param.camera_resolution = sl::RESOLUTION::HD1080;
    init_param.camera_fps = 60;

    run_time_param.sensing_mode = sl::SENSING_MODE::STANDARD;
    run_time_param.enable_depth = false;

    ZED_err = zed.open(init_param);
        if (ZED_err != sl::ERROR_CODE::SUCCESS) {
            std::cout << ZED_err << std::endl;
            return false;
        }

    sl::sleep_ms(50);   //wait for calibration 

    ZED_inf = zed.getCameraInformation();   //read only
    CAM_WINDOW_WIDTH = ZED_inf.camera_configuration.resolution.width;
    CAM_WINDOW_HEIGHT = ZED_inf.camera_configuration.resolution.height;
    std::cout <<"ZED cam resolution: " << CAM_WINDOW_WIDTH<<'x'<< CAM_WINDOW_HEIGHT << std::endl;
    std::cout << "ZEDcam initialization complete" << std::endl;
    return true;

    
}
#endif
bool opngl_init() {
    //матрица проекции
    //ProjectionMat = glm::perspective(glm::radians(100.0f), (float)HMD_WIDTH / (float)HMD_HEIGHT, 0.1f, 2.0f);
    ProjectionMat = glm::mat4(
        VrProjMat.m[0][0], VrProjMat.m[0][1], VrProjMat.m[0][2], VrProjMat.m[0][3],
        VrProjMat.m[1][0], VrProjMat.m[1][1], VrProjMat.m[1][2], VrProjMat.m[1][3],
        VrProjMat.m[2][0], VrProjMat.m[2][1], VrProjMat.m[2][2], VrProjMat.m[2][3],
        VrProjMat.m[3][0], VrProjMat.m[3][1], -0.2f, VrProjMat.m[3][3]);
    ProjectionMat_Ptr = (void*)(glm::value_ptr(ProjectionMat));
    //ProjectionMat_Ptr = (VrProjMat.m);
    //Shader compilation
    char errlog[1024];
    displayShPr = Shader_Program("PlaneShaderVer.txt", "PlaneShaderFr.txt", errlog, 1024);
    if (!displayShPr.err) { std::cout << errlog << std::endl; std::cout << "Failed to initialize" << std::endl; return false; }
    return true;

}


int main()
{   
    // ------------------INITIALIZATION------------------  
    // ------------------INITIALIZATION------------------  
    sf::Clock clock;
    //if (!VR_init()) { return -1; }
    //if (!ZED_init()) { return -1; }
   
    //ZEDcam buffers
    //sl::Mat ZED_image_left = sl::Mat(CAM_WINDOW_WIDTH, CAM_WINDOW_HEIGHT,sl::MAT_TYPE::U8_C4,sl::MEM::CPU);
    //sl::Mat ZED_image_right = sl::Mat(CAM_WINDOW_WIDTH,CAM_WINDOW_HEIGHT,sl::MAT_TYPE::U8_C4,sl::MEM::CPU);
    //std::cout << ZED_image_left.getInfos() << std::endl;
    //std::cout << ZED_image_right.getInfos() << std::endl;

    //left_image_Ptr = (char*)ZED_image_left.getPtr<sl::uchar1>(sl::MEM::CPU);
   // right_image_Ptr = (char*)ZED_image_right.getPtr<sl::uchar1>(sl::MEM::CPU);

    //opengl shared context
    sf::Context context;
    // glad for opengl
    if (!gladLoadGL())
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    if (!opngl_init()) { return -1; }

    // ------------------OPENGL CONTEXT SETUP------------------  
    // ------------------OPENGL CONTEXT SETUP------------------ 
GLuint LeftFrameBuffer;
GLuint RightFrameBuffer;
GLuint LeftEyeTexture;
GLuint RightEyeTexture;
    //opengl left window context
    sf::RenderWindow left_window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "LEFT",sf::Style::Close);
    left_window.setActive(true);
    display_plane display_left = display_plane(&displayShPr, CAM_WINDOW_WIDTH, CAM_WINDOW_HEIGHT, left_image_Ptr, ProjectionMat_Ptr);
    display_plane leftArm1 = display_plane(&displayShPr, CAM_WINDOW_WIDTH, CAM_WINDOW_HEIGHT, left_image_Ptr, ProjectionMat_Ptr);
    display_plane leftArm2 = display_plane(&displayShPr, CAM_WINDOW_WIDTH, CAM_WINDOW_HEIGHT, glm::vec4(0.0f, 0.5f, 0.5f, 1.0f), ProjectionMat_Ptr);
    leftArm1.ScaleVec = glm::vec3(0.9f, 0.9f, 1.0f);
    leftArm2.ScaleVec = glm::vec3(0.9f, 0.9f, 1.0f);
    glGenFramebuffers(1, &LeftFrameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, LeftFrameBuffer);
    glGenTextures(1, &LeftEyeTexture);
    glBindTexture(GL_TEXTURE_2D, LeftEyeTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, HMD_WIDTH, HMD_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, LeftEyeTexture, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) { std::cout << "Left framebuffer error" << std::endl; return -1; }
    std::cout << "Left framebuffer compleat" << std::endl;
    glBindTexture(GL_TEXTURE_2D, 0);    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //opengl right window context
    sf::RenderWindow right_window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "RIGHT", sf::Style::Close);
    right_window.setActive(true);
    display_plane display_right = display_plane(&displayShPr, CAM_WINDOW_WIDTH, CAM_WINDOW_HEIGHT, right_image_Ptr, &ProjectionMat);
    display_plane rightArm = display_plane(&displayShPr, CAM_WINDOW_WIDTH, CAM_WINDOW_HEIGHT, glm::vec4(0.5f, 0.0f, 0.0f, 1.0f), &ProjectionMat);
    rightArm.ScaleVec = glm::vec3(0.1f, 0.1f, 1.0f);

    glGenFramebuffers(1, &RightFrameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, RightFrameBuffer);
    glGenTextures(1, &RightEyeTexture);
    glBindTexture(GL_TEXTURE_2D, RightEyeTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, HMD_WIDTH, HMD_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, RightEyeTexture, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) { std::cout << "Right framebuffer error" << std::endl; return -1; }
    std::cout << "Right framebuffer compleat" << std::endl;
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //vr::TrackedDevicePose_t m_rTrackedDevicePose[vr::k_unMaxTrackedDeviceCount];
    // ------------------MAIN CYCLE------------------  
    // ------------------MAIN CYCLE------------------ 
    while (left_window.isOpen())
    {


        //ZEDcam data input
        ZED_err = zed.grab(run_time_param);
        if (ZED_err == sl::ERROR_CODE::SUCCESS) {
            zed.retrieveImage(ZED_image_left, sl::VIEW::LEFT);
            zed.retrieveImage(ZED_image_right, sl::VIEW::RIGHT);


        } else { std::cout <<"Image grab: " <<ZED_err << std::endl; }
        */

        //SFML event handling
        sf::Event event;
        left_window.setActive(true);
        while (left_window.pollEvent(event))  //no resize
        {
            if (event.type == sf::Event::Closed)
                left_window.close();
        }

        //VR
        //vr::VRCompositor()->WaitGetPoses(m_rTrackedDevicePose, vr::k_unMaxTrackedDeviceCount, NULL, 0);
        //vr::Texture_t leftEyeTexture = { (void*)(uintptr_t)LeftEyeTexture, vr::TextureType_OpenGL, vr::ColorSpace_Gamma };
        //vr::VRCompositor()->Submit(vr::Eye_Left, &leftEyeTexture);
        //vr::Texture_t rightEyeTexture = { (void*)(uintptr_t)RightEyeTexture, vr::TextureType_OpenGL, vr::ColorSpace_Gamma };
        //vr::VRCompositor()->Submit(vr::Eye_Right, &rightEyeTexture);
        UpdateDeviceStates();
        TransformControllerPosToHeadPos();
        leftArm1.ForwardVec = LeftControllerProperties.ForwardVec;
        leftArm1.UpwardVec = glm::normalize(LeftControllerProperties.UpwardVec);
        leftArm2.ForwardVec = LeftControllerProperties.UpwardVec;
        leftArm2.UpwardVec = glm::normalize(LeftControllerProperties.ForwardVec);
        rightArm.ForwardVec = RightControllerProperties.ForwardVec;
        rightArm.UpwardVec = glm::normalize(RightControllerProperties.UpwardVec);
        leftArm1.PositionVec = LeftControllerProperties.PositionVec;
        leftArm2.PositionVec = LeftControllerProperties.PositionVec;
        rightArm.PositionVec = RightControllerProperties.PositionVec;
        //std::cout << " x: "  << LeftControllerProperties.PositionVec.x ;
        //std::cout << " y: " << LeftControllerProperties.PositionVec.y ;
        //std::cout << " z: " << LeftControllerProperties.PositionVec.z << std::endl;

        
        //-------------------left window render--------------------
        left_window.setActive(true);  //activate window context
        glBindFramebuffer(GL_FRAMEBUFFER, LeftFrameBuffer); //rendering in this buffer
        glClearColor(0.2f, 0.0f, 0.2f, 1.0f);
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        display_left.draw();
        leftArm1.draw();
        leftArm2.draw();
        rightArm.draw();
        left_window.setActive(true);   //activating window context again and its buffer as read/write
        glBindFramebuffer(GL_READ_FRAMEBUFFER, LeftFrameBuffer); //activating rendered buffer as read only
        glBlitFramebuffer(0, 0, HMD_WIDTH, HMD_HEIGHT, 0, 0, HMD_WIDTH, HMD_HEIGHT, GL_COLOR_BUFFER_BIT , GL_NEAREST); //Copying from rendered buffer to window buffer
        left_window.display();  
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        //---------right window render--------------
        right_window.setActive(true); //activate window context
        glBindFramebuffer(GL_FRAMEBUFFER, RightFrameBuffer); //rendering in this buffer
        glClearColor(0.2f, 0.0f, 0.2f, 1.0f);
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        display_right.draw();
        rightArm.draw();
        leftArm1.draw();
        leftArm2.draw();
        right_window.setActive(true); //activating window context again and its buffer as read/write
        glBindFramebuffer(GL_READ_FRAMEBUFFER, RightFrameBuffer); //activating rendered buffer as read only
        glBlitFramebuffer(0, 0, HMD_WIDTH, HMD_HEIGHT, 0, 0, HMD_WIDTH, HMD_HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST); //Copying from rendered buffer to window buffer
        right_window.display();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        

        SPF = clock.restart();
     
        //std::cout << "FPS: " << glm::round(1 / SPF.asSeconds()) << std::endl;
    }
    //vr::VR_Shutdown();
    return 0;
}