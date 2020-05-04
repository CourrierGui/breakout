#include <pangolin/post-processor.hpp>

PostProcessor::PostProcessor(
  Shader& shader, unsigned int width,
  unsigned int height) 
  : 
    post_processing_shader(shader),
    texture(), width(width),
    height(height), confuse(false),
    chaos(false), shake(false)
{
  // initialize renderbuffer/framebuffer object
  glGenFramebuffers(1, &MSFBO);
  glGenFramebuffers(1, &FBO);
  glGenRenderbuffers(1, &RBO);
  
  // initialize renderbuffer storage with a multisampled color buffer (don't
  // need a depth/stencil buffer)
  glBindFramebuffer(GL_FRAMEBUFFER, this->MSFBO);
  glBindRenderbuffer(GL_RENDERBUFFER, this->RBO);

  // allocate storage for render buffer object
  glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_RGB, width, height);

  // attach MS render buffer object to framebuffer
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, this->RBO);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    std::cout << "ERROR::POSTPROCESSOR: Failed to initialize MSFBO" << std::endl;

  // also initialize the FBO/texture to blit multisampled color-buffer to; used
  // for shader operations (for postprocessing effects)
  glBindFramebuffer(GL_FRAMEBUFFER, this->FBO);
  texture.generate(width, height, nullptr);

  // attach texture to framebuffer as its color attachment
  glFramebufferTexture2D(
    GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
    GL_TEXTURE_2D, texture.id, 0
  );

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    std::cout << "ERROR::POSTPROCESSOR: Failed to initialize FBO" << std::endl;

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  // initialize render data and uniforms
  init_render_data();
  post_processing_shader.setInteger("scene", 0);
  float offset = 1.0f / 300.0f;
  float offsets[9][2] = {
    { -offset,  offset  },  // top-left
    {  0.0f,    offset  },  // top-center
    {  offset,  offset  },  // top-right
    { -offset,  0.0f    },  // center-left
    {  0.0f,    0.0f    },  // center-center
    {  offset,  0.0f    },  // center - right
    { -offset, -offset  },  // bottom-left
    {  0.0f,   -offset  },  // bottom-center
    {  offset, -offset  }   // bottom-right    
  };

  glUniform2fv(
    glGetUniformLocation(post_processing_shader.id, "offsets"),
    9, (float*)offsets
  );

  int edge_kernel[9] = {
    -1, -1, -1,
    -1,  8, -1,
    -1, -1, -1
  };

  glUniform1iv(glGetUniformLocation(post_processing_shader.id, "edge_kernel"), 9, edge_kernel);
  float blur_kernel[9] = {
    1.0f / 16.0f, 2.0f / 16.0f, 1.0f / 16.0f,
    2.0f / 16.0f, 4.0f / 16.0f, 2.0f / 16.0f,
    1.0f / 16.0f, 2.0f / 16.0f, 1.0f / 16.0f
  };
  glUniform1fv(glGetUniformLocation(post_processing_shader.id, "blur_kernel"), 9, blur_kernel);    
}

void PostProcessor::begin_render() {
  glBindFramebuffer(GL_FRAMEBUFFER, MSFBO);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
}

void PostProcessor::end_render() {
  // now resolve multisampled color-buffer into intermediate FBO to store to
  // texture
  //
  glBindFramebuffer(GL_READ_FRAMEBUFFER, this->MSFBO);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, this->FBO);
  glBlitFramebuffer(
    0, 0, width, height, 0, 0, width, height,
    GL_COLOR_BUFFER_BIT, GL_NEAREST
  );

  // binds both READ and WRITE framebuffer to default framebuffer
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PostProcessor::render(float time) {
  // set uniforms/options
  post_processing_shader.use();
  post_processing_shader.setFloat("time", time);
  post_processing_shader.setInteger("confuse", confuse);
  post_processing_shader.setInteger("chaos", chaos);
  post_processing_shader.setInteger("shake", shake);
  // render textured quad
  glActiveTexture(GL_TEXTURE0);
  texture.bind();	
  glBindVertexArray(this->VAO);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glBindVertexArray(0);
}

void PostProcessor::init_render_data() {
  // configure VAO/VBO
  unsigned int VBO;
  float vertices[] = {
    // pos        // tex
    -1.0f, -1.0f, 0.0f, 0.0f,
    1.0f,  1.0f, 1.0f, 1.0f,
    -1.0f,  1.0f, 0.0f, 1.0f,

    -1.0f, -1.0f, 0.0f, 0.0f,
    1.0f, -1.0f, 1.0f, 0.0f,
    1.0f,  1.0f, 1.0f, 1.0f
  };
  glGenVertexArrays(1, &this->VAO);
  glGenBuffers(1, &VBO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindVertexArray(this->VAO);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}
