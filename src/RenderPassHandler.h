/* 
 * File:   RenderPassHandler.h
 * Author: jaa
 *
 * Created on 23. listopad 2013, 20:27
 */

#ifndef RENDERPASSHANDLER_H
#define	RENDERPASSHANDLER_H

#include <unordered_map>


class RenderPassHandler {
	typedef std::unordered_map<uint, RenderPass *> PassList;
	typedef std::pair<uint, RenderPass *> dataType;
	
	PassList passes;

public:
	RenderPassHandler() {}
	
	~RenderPassHandler() {
	for(PassList::const_iterator it = passes.begin(); it != passes.end(); ++it) {
			if((*it).second) delete (*it).second;	
		}
	}
	
	void draw(ObjectData *object) {
		for(PassList::const_iterator it = passes.begin(); it != passes.end(); ++it) {
			(*it).second->draw(object);		
		}
	}
	
	void add(RenderPass::RenderPassType type, RenderPass * pass) {
		passes.insert(dataType((uint) type, pass));
	}
	
	void remove(RenderPass::RenderPassType type) {
		RenderPass * p = passes.at((uint) type);
		passes.erase(type);
		if(p) delete p;
	}

};



#endif	/* RENDERPASSHANDLER_H */

