/* 
 * File:   TexturingPrePass.h
 * Author: jaa
 *
 * Created on 10. duben 2014, 0:34
 */

#ifndef TEXTURINGPREPASS_H
#define	TEXTURINGPREPASS_H

#include "RenderPass.h"

class TexturingRenderPass;

class TexturingPrePass : public RenderPass {
	friend TexturingRenderPass;
	
	GLuint loc_textureCount;
	GLuint loc_viewDir;
	GLuint textureDataUB;
	
	GLuint frameBuffer;
	GLuint normalsTexture;
	GLuint normalsSampler;
	
	std::vector<float> data;
	
	GLuint reductionShaderID;
	GLuint kmeansShaderID;
	GLuint loc_km_comp_texSize;
	GLuint loc_red_iter;
	GLuint loc_red_comp_texSize;
	GLuint resultBuffer;
	GLuint idxBuffer;
	
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
		
		kmeansShaderID = GL_ID_NONE;
		reductionShaderID = GL_ID_NONE;
		loc_km_comp_texSize = GL_ID_NONE;
		loc_red_iter = GL_ID_NONE;
		resultBuffer = GL_ID_NONE;
		idxBuffer = GL_ID_NONE;
	}
		
	~TexturingPrePass() {
		//TODO delete buffers
	}
	
	virtual void reset() {
		programID = GL_ID_NONE;
		kmeansShaderID = GL_ID_NONE;
		reductionShaderID = GL_ID_NONE;
	}
	
	void draw(std::shared_ptr<ObjectData> object) {
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glEnable(GL_TEXTURE_2D);
		glDisable(GL_BLEND);
		
		
		assert(textureHandler);
		//std::vector<Texture> * textures = &textureHandler->getTextures();
		
		const uint sizeOfTextureData = sizeof(glm::mat4) + sizeof(glm::ivec2) + 2 * sizeof(float);
		if(programID == GL_ID_NONE) {
			programID = shaderHandler.getProgramId(shader);
			getDefaultUniformLocations();
			
			loc_textureCount = glGetUniformLocation(programID, "u_textureCount");
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
		
		const glm::mat4 invMvm = glm::transpose(object->getMvm());
		glm::vec3 viewDir = glm::normalize(object->getCentroidPosition() - renderer.getCamera().getCameraPosition());
		glm::vec3 viewDirObjSpace(glm::normalize(invMvm * glm::vec4(viewDir, 1.0f)));
		const glm::vec3 camPos(glm::inverse(object->getMvm()) * glm::vec4(renderer.getCamera().getCameraPosition(), 1.0f));

		std::vector<Photo *> photos = textureHandler->getClosestCameras(viewDirObjSpace, Settings::usingTextures); 
				
		if(textureDataUB != GL_ID_NONE) {
			glBindBuffer(GL_UNIFORM_BUFFER, textureDataUB);
			int offset = 0;
			for(uint i = 0; i < photos.size(); ++i) { //slooooooooow
				const Photo *p = photos.at(i);
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
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, winSize.x, winSize.y, 0, GL_RGB, GL_FLOAT, NULL);
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
		
		GLint texCount = photos.size();
		glUniform1i(loc_textureCount, texCount);
				
		glUniform3fv(loc_viewDir, 1, &viewDir[0]);
		
		renderer.setUniformLocations(&uLocs);
		renderer.bindCameraMatrices();
		renderer.drawObject(*object);
		
		glUseProgram(0);
		
		if(true) { //TMP DEBUG
		
		if(kmeansShaderID == GL_ID_NONE) {
			kmeansShaderID = shaderHandler.getProgramId(ShaderHandler::SHADER_COMPUTE_KMENAS);
			loc_km_comp_texSize = glGetUniformLocation(kmeansShaderID, "u_texSize");
		}
		if(reductionShaderID == GL_ID_NONE) {
			reductionShaderID = shaderHandler.getProgramId(ShaderHandler::SHADER_COMPUTE_REDUCTION);
			loc_red_comp_texSize = glGetUniformLocation(reductionShaderID, "u_texSize");
			loc_red_iter = glGetUniformLocation(reductionShaderID, "u_iteration");		
		}
		
		const int sizeOfresult = 20 * sizeof(GLfloat) + sizeof(GLboolean); //TODO
		const float blockSize = 16.f;
		const glm::ivec2 gridSize(ceil(winSize.x/blockSize), ceil(winSize.y/blockSize));
		const int sizeofInd = gridSize.x * gridSize.y * 256 * sizeof(GLuint);
		
		if(resultBuffer == GL_ID_NONE) {
			glGenBuffers(1, &resultBuffer);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, resultBuffer);
			glBufferData(GL_SHADER_STORAGE_BUFFER, sizeOfresult, NULL, GL_STATIC_DRAW);
		}
		if(idxBuffer == GL_ID_NONE) {
			glGenBuffers(1, &idxBuffer);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, idxBuffer);
			std::vector<uint> init(gridSize.x * gridSize.y * 256);
			std::fill(init.begin(), init.end(), 6);
			glBufferData(GL_SHADER_STORAGE_BUFFER, sizeofInd, &init[0], GL_STATIC_DRAW);
		}
				
		struct Cl {
			glm::vec3 cntr;
			uint size;
		};
		
		int iter = 0;
		bool moving = true;
		while(moving && iter < 2) {
			iter++;
			glUseProgram(reductionShaderID);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, normalsTexture);
			glBindSampler(0, normalsSampler);

			glUniform2iv(loc_red_comp_texSize, 1, &winSize[0]);
			glUniform1iv(loc_red_iter, 1, &iter);

			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, resultBuffer);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, idxBuffer);

			glDispatchCompute(gridSize.x, gridSize.y, 1);
			glCheckError();

			glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_BUFFER_UPDATE_BARRIER_BIT);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, resultBuffer);
			Cl* data = (Cl*) glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_WRITE);
			for(int i = 0; i < 5; ++i) {
				data[i].cntr /= data[i].size;
				data[i].cntr = glm::normalize(data[i].cntr);
			}
			glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
			
			glUseProgram(kmeansShaderID);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, normalsTexture);
			glBindSampler(0, normalsSampler);

			glUniform2iv(loc_km_comp_texSize, 1, &winSize[0]);	
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, resultBuffer);	
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, idxBuffer);
			
			glDispatchCompute(gridSize.x, gridSize.y, 1);
			glCheckError();
			glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_BUFFER_UPDATE_BARRIER_BIT);
			
			glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, sizeOfresult - sizeof(GLboolean), sizeof(GLboolean), &moving);
		}
//		std::cout << "iter: " << iter << "\n"; 
		std::vector<Cluster> clusters;
		
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, resultBuffer);
		Cl* data = (Cl*) glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_WRITE);
		int sum;
		for(int i = 0; i < 5; ++i) {
			if(data[i].size != 0) {
				Cluster c;
				c.id = clusters.size();
				c.centroid = data[i].cntr;
				c.size = data[i].size;
				clusters.push_back(c);
			}
			sum += data[i].size;
		}
		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		
//		glBindTexture(GL_TEXTURE_2D, normalsTexture);
//		if(data.size() < (uint) 3* winSize.x * winSize.y) {
//			data.resize(3* winSize.x * winSize.y);
//		}
//		glReadPixels(0, 0, winSize.x, winSize.y, GL_RGB,  GL_FLOAT, &data[0]);
//		
//		const int clusterCount = 5;
//		typedef std::pair<int, glm::vec3> Norm;
//		std::vector<Norm> normals;
////		normals.reserve(data.size() / 3);
//		
//		std::vector<Cluster> clusters;
//		clusters.resize(clusterCount);
//		for(int i = 0; i < clusterCount; ++i) {
//			clusters[i].id = i;
//			clusters[i].size = 0;
//			clusters[i].weight = 0;
//		}
//		
//		
//		std::default_random_engine gen;
//		std::uniform_int_distribution<int> distr(0, clusterCount-1);
//		
//		
//		for(uint i = 0; i < data.size(); i += 3) {
//			if(data[i] != 0.f || data[i+1] != 0.f || data[i+2] != 0.f) {
//				int d = distr(gen); //random is OK, it's k-menas works fast...
//				normals.push_back(Norm(d, glm::vec3(data[i], data[i+1], data[i+2])));
//			}
//		}
//		
//		bool moving = true;
//		int iterations = 0;
//		while(moving && iterations < 20) {
//			moving = false;
//			for(Cluster &c : clusters) {
//				c.centroid *= 0;
//				c.size = 0;
//			}
//			
//			for(Norm &n : normals) {
//				clusters[n.first].centroid += n.second;
//				clusters[n.first].size++;
//			}
//			for(Cluster &c : clusters) {
//				c.centroid /= (float) c.size;
//				c.centroid = glm::normalize(c.centroid);
//			}
//		
//			float dist = -1;
//			for(Norm &n : normals) {
//				dist = glm::dot(n.second, clusters[n.first].centroid);
//				int moveTo = -1;
//				for(Cluster &c : clusters) {
//					const float diff = glm::dot(n.second, c.centroid);
//					if(diff > dist) {
//						moveTo = c.id;
//						dist = diff; // !?
//					}
//					else if(diff < dist - 0.05) { //merge similar clusters
//						if(c.id < n.first) {
//							moveTo = c.id;
//							dist = diff; // !?
//						}
//					}
//				}
//				if(moveTo != -1) {
//					n.first = moveTo;
//					moving = true;
//				}
//			}
//			++iterations;
//		}
//		for(Cluster &c : clusters) {
//			c.centroid *= 0;
//			c.size = 0;
//		}
//		for(Norm &n : normals) {
//			clusters[n.first].centroid += n.second;
//			clusters[n.first].size++;
//		}
		
		for(Cluster &cl: clusters) {
			cl.centroid /= (float) cl.size;
			cl.centroid *= -1;
			cl.centroid = glm::normalize(cl.centroid);
			
			cl.weight = cl.size / (float) sum;
		}

		std::sort(clusters.begin(), clusters.end(), 
				[] (const Cluster &a, const Cluster &b) {
					return a.weight > b.weight;
				}
		);
		
		textureHandler->setClusters(clusters);
		}
		textureHandler->updateTextures(viewDirObjSpace, Settings::usingTextures);
		
		glCheckError();
		glUseProgram(0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glDisable(GL_TEXTURE_2D);
	}

};

#endif	/* TEXTURINGPREPASS_H */

