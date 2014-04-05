/* 
 * File:   RenderPassHandler.h
 * Author: jaa
 *
 * Created on 23. listopad 2013, 20:27
 */

#ifndef RENDERPASSHANDLER_H
#define	RENDERPASSHANDLER_H

#include <map>


class RenderPassHandler {
	//typedef std::unordered_map<uint, RenderPass *> PassList;
	typedef std::map<uint, RenderPass *> PassList;
	typedef std::pair<uint, RenderPass *> dataType;
	
	PassList passes;

public:
	RenderPassHandler() {}
	
	~RenderPassHandler() {
		for(auto &it : passes) {
			if(it.second) delete it.second;	
		}
	}
	
	void draw(std::shared_ptr<ObjectData> object) {
		for(auto &it : passes) {
			it.second->draw(object);		
		}
	}
	
	void add(RenderPass::RenderPassType type, RenderPass * pass) {
		passes.insert(dataType((uint) type, pass));
	}
	
	void remove(RenderPass::RenderPassType type) {
		RenderPass * p = passes.at((uint) type);
		passes.erase(type);
		DELETE(p);
	}
	
	void removeAll() {
		for(dataType p :passes) {
			DELETE(p.second);
		}
		passes.clear();
	}
	
	void resetShaderIDs() {
		for(auto &it : passes) {
			it.second->reset();
		}	
	}

};



#endif	/* RENDERPASSHANDLER_H */

