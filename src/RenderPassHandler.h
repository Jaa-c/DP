/** @file 
 * File:   RenderPassHandler.h
 * Author: Daniel Pinc <princdan@fel.cvut.cz>
 *
 * Created on 23. listopad 2013, 20:27
 */

#ifndef RENDERPASSHANDLER_H
#define	RENDERPASSHANDLER_H

#include <map>

#include "RenderPass/RenderPass.h"

/**
 * Handles render passes and their order
 */
class RenderPassHandler {
	typedef std::pair<uint, std::shared_ptr<RenderPass>> dataType;
	typedef std::map<uint, std::shared_ptr<RenderPass>> PassList;
	
	PassList passes; //!< list of current render passes

public:
	RenderPassHandler() {}
	
	/**
	 * Delegates draw to used render passes
     * @param object
     */
	void draw(std::shared_ptr<ObjectData> object) {
		for(auto &it : passes) {
			it.second->draw(object);		
		}
	}
	/// ads render pass to pipeline
	void add(RenderPass::RenderPassType type, std::shared_ptr<RenderPass> pass) {
		passes.insert(dataType((uint) type, pass));
	}
	
	/// removes render pass from pipeline
	void remove(RenderPass::RenderPassType type) {
		passes.at(type)->reset();
		passes.erase(type);
	}
	
	/// removes all render passes
	void removeAll() {
		passes.clear();
	}
	
	/// resets shaders, DEBUG
	void resetShaderIDs() {
		for(auto &it : passes) {
			it.second->reset();
		}	
	}

};



#endif	/* RENDERPASSHANDLER_H */

