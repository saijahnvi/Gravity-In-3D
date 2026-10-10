#include "vertexbuffer.h"
#include "Renderer.h"
#include <iostream>

vertexbuffer::vertexbuffer(const void*data,unsigned int size){
 glGenBuffers(1, &m_RendererID);
    glBindBuffer(GL_ARRAY_BUFFER,m_RendererID);
    glBufferData(GL_ARRAY_BUFFER,size,data , GL_STATIC_DRAW);

}
vertexbuffer::~vertexbuffer(){
glDeleteBuffers(1,&m_RendererID);
}
void vertexbuffer::Bind() const {
    glBindBuffer(GL_ARRAY_BUFFER,m_RendererID);
    
}
void vertexbuffer::UnBind() const{
      glBindBuffer(GL_ARRAY_BUFFER,0);
    
}