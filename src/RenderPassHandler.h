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
	typedef std::pair<uint, std::shared_ptr<RenderPass>> dataType;
	typedef std::map<uint, std::shared_ptr<RenderPass>> PassList;
	
	PassList passes;

public:
	RenderPassHandler() {}
	
	void draw(std::shared_ptr<ObjectData> object) {
		for(auto &it : passes) {
			it.second->draw(object);		
		}
	}
	
	void add(RenderPass::RenderPassType type, std::shared_ptr<RenderPass> pass) {
		passes.insert(dataType((uint) type, pass));
	}
	
	void remove(RenderPass::RenderPassType type) {
		passes.erase(type);
	}
	
	void removeAll() {
		passes.clear();
	}
	
	void resetShaderIDs() {
		for(auto &it : passes) {
			it.second->reset();
		}	
	}

};



#endif	/* RENDERPASSHANDLER_H */

