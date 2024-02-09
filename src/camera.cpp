#include <ebb/camera.hpp>

void Ebb::Camera::save(FILE *file) {
    this->Ebb::Node::save(file);
    this->_texture->save(file);

    fwrite(&this->fov, 1, sizeof(float), file);
    fwrite(&this->aspect, 1, sizeof(float), file);
    fwrite(&this->nearclip, 1, sizeof(float), file);
    fwrite(&this->farclip, 1, sizeof(float), file);
}
void Ebb::Camera::load(FILE *file) {
    this->Ebb::Node::load(file);
    this->_texture->load(file);

    fread(&this->fov, 1, sizeof(float), file);
    fread(&this->aspect, 1, sizeof(float), file);
    fread(&this->nearclip, 1, sizeof(float), file);
    fread(&this->farclip, 1, sizeof(float), file);

    this->proj = Ebb::Math::perspective(this->fov, (float)this->_texture->width() / (float)this->_texture->height(), this->nearclip, this->farclip);
}