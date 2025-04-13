#include "pawnTree.hpp"
#include "shared/logger.hpp"

/*
 * PawnTree
 */

PawnTree::PawnTree() {
	root = std::make_shared<RootPawn>();
	//TODO adding root to hashmap?
	root->pawn_state = PawnState::TRACKED;
}

std::shared_ptr<Pawn> PawnTree::findByName(const std::string& name) {
	std::unordered_multimap<std::string, std::shared_ptr<Pawn>>::iterator result = nameMap.find(name);
	if (result != nameMap.end()) {
		return result->second;
	}
	return nullptr;
}

std::shared_ptr<Pawn> PawnTree::findByID(uint32_t id) {
	std::unordered_multimap<uint32_t, std::shared_ptr<Pawn>>::iterator result = idMap.find(id);
	if (result != idMap.end()) {
		return result->second;
	}
	return nullptr;
}

bool PawnTree::removeFromMaps(const std::string &name, uint32_t id) {

	int erasedAmount = 0;

	auto results = nameMap.equal_range(name);

	for (auto it = results.first; it != results.second;) {
		if(it->second->id == id){
			it = nameMap.erase(it);
			erasedAmount++;
			continue;
		}
		it ++;
	}

	if(erasedAmount == 1){

	}
	else{
		FAULT("There should be only one entity with given name and ID");
	}

	idMap.erase(idMap.find(id));

	return true;
}

std::vector<std::shared_ptr<Pawn>> PawnTree::findAllByName(const std::string& name) {
	std::vector<std::shared_ptr<Pawn>> result;
	auto range = nameMap.equal_range(name);
	for (auto r = range.first; r != range.second; ++r) {
		result.push_back(r->second);
	}
	return result;
}

std::vector<std::shared_ptr<Pawn>> PawnTree::findAllByID(uint32_t id) {
	std::vector<std::shared_ptr<Pawn>> result;
	auto range = idMap.equal_range(id);
	for (auto r = range.first; r != range.second; ++r) {
		result.push_back(r->second);
	}
	return result;
}

size_t PawnTree::nameHitSize(const std::string& name) {
	return nameMap.count(name);
}

size_t PawnTree::idHitSize(uint32_t id) {
	return idMap.count(id);
}

void PawnTree::addToRoot(std::shared_ptr<Pawn> pawn) {
	root->addChild(pawn);
	mountPawn(pawn);
}

void PawnTree::mountPawn(std::shared_ptr<Pawn> pawn) {
	bool isCopy = false;
	bool isChanged = pawn->unregisteredChildAdded();

	if (idMap.count(pawn->getEntityID()) > 0) {
		auto range = idMap.equal_range(pawn->getEntityID());
		for (auto r = range.first; r != range.second; ++r) {
			if (r->second == pawn) {
				isCopy = true;
			}
		}
	}

	//TODO test if it works
	if (!isCopy) {
		std::string p_name = pawn->getName();
		uint32_t p_id = pawn->getEntityID();
		addPawnToHash(p_name,p_id,pawn);
	}

	if (isChanged) {
		updatePawnsChildren(pawn);
		isChanged = false;
	}
}

void PawnTree::updatePawnsChildren(std::shared_ptr<Pawn>& pawn) {
	std::vector<std::shared_ptr<Pawn>> pawn_children = pawn->getChildren();
	for (std::shared_ptr<Pawn> pawn_child : pawn_children) {
		mountPawn(pawn_child);
	}
}

std::shared_ptr<RootPawn> PawnTree::getRoot() {
	return root;
}

void PawnTree::updateTree(double delta) {
	std::vector<std::shared_ptr<Pawn>> pawn_children = root->getChildren();
	for (std::shared_ptr<Pawn> pawn_child : pawn_children) {
		updateTreeRecursion(pawn_child, delta);
	}
}

void PawnTree::updateTreeRecursion(std::shared_ptr<Pawn> pawn_to_update, double delta) {
	pawn_to_update->onUpdate(delta);

	//TODO maybe create iterator of some kind with lambda
	std::vector<std::shared_ptr<Pawn>> pawn_children = pawn_to_update->getChildren();
	for (std::shared_ptr<Pawn> pawn_child : pawn_children) {
		updateTreeRecursion(pawn_child, delta);
	}
}

void PawnTree::fixedUpdateTree() {
	std::vector<std::shared_ptr<Pawn>> pawn_children = root->getChildren();
	for (std::shared_ptr<Pawn> pawn_child : pawn_children) {
		fixedUpdareTreeRecursion(pawn_child);
	}
}

void PawnTree::fixedUpdareTreeRecursion(std::shared_ptr<Pawn> pawn_to_fixed_update) {
	pawn_to_fixed_update->onFixedUpdate();

	//TODO maybe create iterator of some kind with lambda
	std::vector<std::shared_ptr<Pawn>> pawn_children = pawn_to_fixed_update->getChildren();
	for (std::shared_ptr<Pawn> pawn_child : pawn_children) {
		fixedUpdareTreeRecursion(pawn_child);
	}
}

std::string PawnTree::toString() {
	return printStart(false);
}


std::string PawnTree::toStringVerbose() {
	return printStart(true);
}

std::string PawnTree::printStart(bool verbose) {
	std::string result;
	std::string n;
	if(!verbose) n = root->toString();
	else n = root->toStringVerbose();
	result += "Root:" + n + "\n";
	for (const auto& c : root->getChildren()) {
		result += recursiveString(c, 1, verbose);
	}
	return result;
}


std::string PawnTree::recursiveString(std::shared_ptr<Pawn> p, int depth, bool verbose) {
	std::string result = "";
	for (int i = 0; i < depth; i++) {
		result += "  ";
	}

	std::string n;
	if(!verbose) n = p->toString();
	else n = p->toStringVerbose();

	result += "Pawn:" + n + "\n";

	if(verbose){
		for (auto component : p->components){
			for (int i = 0; i <= depth; i++) {
				result += "  ";
			}
			result += "Component:" + component->toString() + "\n";
		}
	}

	std::vector<std::shared_ptr<Pawn>> p_children = p->getChildren();
	for (size_t i = 0; i < p_children.size(); i++) {
		result += recursiveString(p_children[i], depth + 1, verbose);
	}
	return result;
}

void PawnTree::addPawnToHash(const std::string& p_name, uint32_t p_id, std::shared_ptr<Pawn> pawn) {
	nameMap.insert(std::pair<std::string, std::shared_ptr<Pawn>>(p_name, pawn));
	idMap.insert(std::pair<uint32_t, std::shared_ptr<Pawn>>(p_id, pawn));
	pawn->setTracked(true);
}



