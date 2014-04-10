/* 
 * File:   TexturingPrePass.h
 * Author: jaa
 *
 * Created on 10. duben 2014, 0:34
 */

#ifndef TEXTURINGPREPASS_H
#define	TEXTURINGPREPASS_H

#include "RenderPass.h"

class TexturingPrePass : public RenderPass {
		
	GLuint loc_textureCount;
	GLuint loc_textureIndices;
	GLuint loc_viewDir;
	GLuint textureDataUB;
	
	GLuint frameBuffer;
	GLuint normalsTexture;
	GLuint normalsSampler;
	
public:
	
	TexturingPrePass(
		Renderer& r, 
		ShaderHandler& s, 
		std::shared_ptr<TextureHandler> th
	) : 
		RenderPass(TEXTURING_PRE_PASS, r, s, th)
	{	
		shader = ShaderHandler::SHADER_TEXTURING_1;
		textureDataUB = GL_ID_NONE;
		frameBuffer = GL_ID_NONE;
		normalsTexture = GL_ID_NONE;
		normalsSampler = GL_ID_NONE;
		
		loc_viewDir = GL_ID_NONE;
	}
		
	~TexturingPrePass() {

	}
	
	void draw(std::shared_ptr<ObjectData> object) {
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glEnable(GL_TEXTURE_2D);
		glDisable(GL_BLEND);
		
		const uint sizeOfTextureData = sizeof(glm::mat4) + sizeof(glm::ivec2) + 2 * sizeof(float);
		if(programID == GL_ID_NONE) {
			programID = shaderHandler.getProgramId(shader);
			getDefaultUniformLocations();
			
			loc_textureCount = glGetUniformLocation(programID, "u_textureCount");
			loc_textureIndices = glGetUniformLocation(programID, "u_textureIndices");
			loc_viewDir = glGetUniformLocation(programID, "u_viewDir");
			textureDataUB = GL_ID_NONE;
			
			// the binding point must be smaller than GL_MAX_UNIFORM_BUFFER_BINDINGS
			GLuint bindingPoint = 1, blockIndex; 
			blockIndex = glGetUniformBlockIndex(programID, "u_textureDataBlock");
			if(blockIndex == GL_INVALID_INDEX) { //block not found
				Log::e("TexturingRenderPass: invalid u_textureDataBlock index");
			}
			else {
				glUniformBlockBinding(programID, blockIndex, bindingPoint);
				glGenBuffers(1, &textureDataUB);
				glBindBuffer(GL_UNIFORM_BUFFER, textureDataUB);
				glBufferData(GL_UNIFORM_BUFFER, 32 * sizeOfTextureData, NULL, GL_STATIC_DRAW);
				glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, textureDataUB);
			}
		}
				
		assert(programID != GL_ID_NONE);
		glUseProgram(programID);
		
		assert(textureHandler);
		std::vector<Texture> * textures = &textureHandler->getTextures();
		
		//this needs optimization later!!
		if(textureDataUB != GL_ID_NONE) {
			glBindBuffer(GL_UNIFORM_BUFFER, textureDataUB);
			int offset = 0;
			for(uint i = 0; i < textures->size(); ++i) { //slooooooooow
				const Photo *p = textures->at(i).photo;
				glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(glm::mat4), &p->camera.Rt[0][0]);
				offset += 16 * sizeof(float);
				glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(glm::ivec2) , &p->getImage().size);
				offset += 2 * sizeof(int);
				float focalL = p->camera.focalL / p->getImageScale(); //changing focal length for thumbnails
				glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(float), &focalL);
				offset += 2 * sizeof(float); //note that this is std140 alignment!
			}
			glBindBuffer(GL_UNIFORM_BUFFER, 0);
		}
		
		
		//1st pass:
			
		const glm::ivec2 winSize = renderer.getCamera().getWindowSize();
		if(normalsTexture == GL_ID_NONE) {
			glGenTextures(1, &normalsTexture);
			glBindTexture(GL_TEXTURE_2D, normalsTexture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, winSize.x, winSize.y, 0, GL_RGB, GL_FLOAT, NULL);
		}
		if(normalsSampler == GL_ID_NONE) {
			glGenSamplers(1, &normalsSampler);
			glSamplerParameteri(normalsSampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glSamplerParameteri(normalsSampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glSamplerParameteri(normalsSampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			glSamplerParameteri(normalsSampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		}
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, normalsTexture);
		glBindSampler(0, normalsSampler);
		
		if(frameBuffer == GL_ID_NONE) {
			glGenFramebuffers(1, &frameBuffer);
		}
		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, normalsTexture, 0);
		glClearColor(0.f, 0.f, 0.f, 0.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
		glDrawBuffers(1, DrawBuffers); 
		
		if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			Log::e("[Texturing render pass] framebuffer NOT OK");
			throw "framebuffer error, can't render textures!";
		}
		
		GLint texCount = textures->size();
		glUniform1i(loc_textureCount, texCount);
		
		std::vector<int> &indices = textureHandler->getBestTexIdx();
		glUniform1iv(loc_textureIndices, indices.size(), &indices[0]);
		
		glm::vec3 viewDir = object->getCentroidPosition() - renderer.getCamera().getCameraPosition();
		glUniform3fv(loc_viewDir, 1, &viewDir[0]);
		
		renderer.setUniformLocations(&uLocs);
		renderer.bindCameraMatrices();
		renderer.drawTextures(textures);
		renderer.drawObject(*object);
		
		glBindTexture(GL_TEXTURE_2D, normalsTexture);
		std::vector<float> data(3* winSize.x * winSize.y);
		glReadPixels(0, 0, winSize.x, winSize.y, GL_RGB,  GL_FLOAT, &data[0]);
		
		const int clusterCount = 5;
		std::vector<glm::vec3> normals;
		struct Cluster {
			glm::vec3 centroid;
			std::vector<const glm::vec3 *> vectors;
			float weight;
		};
		std::vector<Cluster> clusters;
		clusters.resize(clusterCount);
		
		
		std::default_random_engine gen;
		std::uniform_int_distribution<int> distr(0, clusterCount-1);
		
		
		for(uint i = 0; i < data.size(); i += 3) {
			if(data[i] != 0.f) {
				int d = distr(gen); //random is OK, it's k-menas works fast...
				normals.push_back(glm::vec3(data[i], data[i+1], data[i+2]));
				clusters[d].vectors.push_back(&normals.at(normals.size()-1));
			}
		}
		bool moving = true;
		int iterations = 0;
		while(moving && iterations < 20) {
			moving = false;
			
			for(Cluster &c : clusters) {
				c.centroid *= 0;
				if(!c.vectors.empty()) {
					for(auto *v : c.vectors) {
						c.centroid += *v;
					}
					c.centroid /= (float) c.vectors.size();
					c.centroid = glm::normalize(c.centroid);
				}
			}
		
			float dist = -1;
			for(Cluster &c : clusters) {
				for(auto v = c.vectors.begin(); v != c.vectors.end();) {
					dist = glm::dot(**v, c.centroid);
//					if(c.vectors.size() == 1) {
//						dist = 0;
//					}
					Cluster *moveTo = nullptr;
					for(Cluster &cl: clusters) {
						float diff = glm::dot(**v, cl.centroid);
						if(diff > dist + 0.001) {
							moveTo = &cl;
						}
					}
					if(moveTo) {
						moveTo->vectors.push_back(*v);
						v = c.vectors.erase(v);
						moving = true;
					}
					else {
						++v;
					}
				}
			}
			++iterations;
			for(Cluster &cl: clusters) {
				cl.weight = 0;
				for(auto v : cl.vectors) {
					cl.weight += glm::dot(*v, cl.centroid);
				}
				cl.weight /= (float) cl.vectors.size();
			}
		}
		
		std::sort(clusters.begin(), clusters.end(), 
				[] (const Cluster &a, const Cluster &b) {
					return a.vectors.size() > b.vectors.size();
				}
		);
		for(Cluster &c : clusters) {
			c.weight = c.vectors.size() / (float) normals.size();
			c.centroid = glm::normalize(c.centroid);
		}
//		std::cout << "Clusters found in " << iterations << " iterations\n";
		
		glCheckError();
		glUseProgram(0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glDisable(GL_TEXTURE_2D);
	}

};

#endif	/* TEXTURINGPREPASS_H */

