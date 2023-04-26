#include <display_plane.h>

display_plane::display_plane(Shader_Program *ShProg_,unsigned int PixelWidth_, unsigned int PixelHeight_, char* image_, void* ProjectionMat_) {
    this->IsTextureUse = true;
    this->ShProg = ShProg_;
    this->image = image_;
    this->PixelWidth = PixelWidth_;
    this->PixelHeight = PixelHeight_;
    this->ProjectionMat = ProjectionMat_;
    float ratio =  (float)(this->PixelHeight) / (float)(this->PixelWidth);
    this->InitialScaleMat = glm::scale(this->Mat, glm::vec3(-1.0f, -ratio, 1.0f));
    //this->ScaleVec = glm::vec3((float)(glm::tan((float)glm::radians(45.0f/2.0f))*2.0f), (float)(glm::tan((float)glm::radians(45.0f / 2.0f)) * 2.0f),1.0f);
    this->setup();
    this->TransformMatUpdate();
}
display_plane::display_plane(Shader_Program* ShProg_, unsigned int PixelWidth_, unsigned int PixelHeight_, glm::vec4 MyColor_, void* ProjectionMat_) {
    this->IsTextureUse = false;
    this->ShProg = ShProg_;
    this->image = nullptr;
    this->MyColor = MyColor_;
    this->PixelWidth = PixelWidth_;
    this->PixelHeight = PixelHeight_;
    this->ProjectionMat = ProjectionMat_;
    float ratio = (float)(this->PixelHeight) / (float)(this->PixelWidth);
    this->InitialScaleMat = glm::scale(this->Mat, glm::vec3(-1.0f, -ratio, 1.0f));
    this->ScaleVec = glm::vec3((float)(glm::tan((float)glm::radians(45.0f / 2.0f)) * 2.0f), (float)(glm::tan((float)glm::radians(45.0f / 2.0f)) * 2.0f), 1.0f);
    this->setup();
    this->TransformMatUpdate();
}
void display_plane::setup() {
    //plane setup
    glGenVertexArrays(1, &(this->VAO));
    glGenBuffers(1, &(this->VBO));
    glBindVertexArray(this->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(this->vertices), this->vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //texture setup
    if (this->IsTextureUse) {
        glGenTextures(1, &(this->TextureID));
        glBindTexture(GL_TEXTURE_2D, this->TextureID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, this->PixelWidth, this->PixelHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, this->image);
        //std::cout << glGetError() << std::endl;
        //(*(this->ShProg)).use();
        //glUniform1i(glGetUniformLocation(this->TextureID, "MyTexture"), 0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}
void display_plane::TransformMatUpdate() {
    
    this->TranslateMat = glm::translate(this->Mat, this->PositionVec);
    this->ScaleMat = glm::scale(this->Mat, this->ScaleVec);
    //this->RotateMat = glm::lookAtRH(glm::vec3(0), this->ForwardVec, this->UpwardVec);
    this->RotateMat = computeRotateMat(this->ForwardVec, this->UpwardVec);
    this->TransformMat = this->TranslateMat * this->RotateMat * this->ScaleMat * this->InitialScaleMat;
    this->TransformMat = this->TranslateMat * this->RotateMat;


}

void display_plane::draw() {
    this->TransformMatUpdate();
    //std::cout << glGetError() << std::endl;
    (*(this->ShProg)).use();
    //std::cout << glGetError() << std::endl;
    glBindVertexArray(this->VAO);
    //std::cout << glGetError() << std::endl;
    if (this->IsTextureUse) {
        glActiveTexture(GL_TEXTURE0);
        //std::cout << glGetError() << std::endl;
        glBindTexture(GL_TEXTURE_2D, this->TextureID);
        //std::cout << glGetError() << std::endl;
        //--------------------------------------------------------оптимизация??
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, this->PixelWidth, this->PixelHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, this->image);
        //std::cout << glGetError() << std::endl;
        glUniform1i(glGetUniformLocation((this->ShProg)->ID, "MyTexture"), 0);
        //std::cout << glGetError() << std::endl;
    }
    glUniformMatrix4fv(glGetUniformLocation((this->ShProg)->ID, "TransformMat"),1, GL_FALSE, glm::value_ptr(this->TransformMat));
    glUniformMatrix4fv(glGetUniformLocation((this->ShProg)->ID, "ProjectionMat"), 1, GL_FALSE, (float*)this->ProjectionMat);
    glUniform4fv(glGetUniformLocation((this->ShProg)->ID, "Color"), 1, glm::value_ptr(this->MyColor));
    glUniform1i(glGetUniformLocation((this->ShProg)->ID, "IsTexture"), this->IsTextureUse);
    //std::cout << glGetError() << std::endl;
    glDrawArrays(GL_TRIANGLES, 0, 6);
    //std::cout << glGetError() << std::endl;
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

glm::mat4 display_plane::computeRotateMat(glm::vec3 forward, glm::vec3 up_) {
    glm::vec3 backward = glm::normalize(forward);
    glm::vec3 up = glm::normalize(up_);
    glm::vec3 right = glm::normalize(glm::cross(forward, up_));
    glm::mat4 mat = glm::mat4(
        right.x, up.x, backward.x, 0,
        right.y, up.y, backward.y, 0,
        right.z, up.z, backward.z, 0,
        0, 0, 0, 1);
    /*std::cout << mat[0][0] << mat[0][1] << mat[0][2] << mat[0][3] << std::endl;
    std::cout << mat[1][0] << mat[1][1] << mat[1][2] << mat[1][3] << std::endl;
    std::cout << mat[2][0] << mat[2][1] << mat[2][2] << mat[2][3] << std::endl;
    std::cout << mat[3][0] << mat[3][1] << mat[3][2] << mat[3][3] << std::endl;*/
    return mat;
}