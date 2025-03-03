#include "pawnTree.hpp"

/*
 * PawnTree
 */

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

std::vector<std::shared_ptr<Pawn>> PawnTree::findAllByName(const std::string& name) {
	std::vector<std::shared_ptr<Pawn>> result;
	auto range = nameMap.equal_range(name);
	for (auto r = range.first; r != range.second; ++r) {
		result.push_back(r->second);
	}
	return std::move(result);
}

std::vector<std::shared_ptr<Pawn>> PawnTree::findAllByID(uint32_t id) {
	std::vector<std::shared_ptr<Pawn>> result;
	auto range = idMap.equal_range(id);
	for (auto r = range.first; r != range.second; ++r) {
		result.push_back(r->second);
	}
	return std::move(result);
}

size_t PawnTree::nameHitSize(const std::string& name) {
	return nameMap.count(name);
}

size_t PawnTree::idHitSize(uint32_t id) {
	return idMap.count(id);
}

void PawnTree::addToRoot(std::shared_ptr<Pawn> pawn) {
	root.addChild(pawn);
	mountPawn(pawn);
}

void PawnTree::mountPawn(std::shared_ptr<Pawn> pawn) {
	bool isCopy = false;
	bool isChanged = pawn->isStructureChanged();
	if (idMap.count(pawn->getEntityID()) > 0) {
		auto range = idMap.equal_range(pawn->getEntityID());
		for (auto r = range.first; r != range.second; ++r) {
			if (r->second == pawn) {
				isCopy = true;
			}
		}
	}

    //TODO test if it works
	if (isCopy == false) {
		std::string p_name = pawn->getName();
		uint32_t p_id = pawn->getEntityID();
		nameMap.insert(std::pair<std::string, std::shared_ptr<Pawn>>(p_name, pawn));
		idMap.insert(std::pair<uint32_t, std::shared_ptr<Pawn>>(p_id, pawn));
	}

	if (isChanged) {
		updatePawnsChildren(pawn);
	}
}

void PawnTree::updatePawnsChildren(std::shared_ptr<Pawn>& pawn) {
	std::vector<std::shared_ptr<Pawn>> pawn_children = pawn->getChildren();
	for (std::shared_ptr<Pawn> pawn_child : pawn_children) {
		mountPawn(pawn_child);
	}
}

RootPawn PawnTree::getRoot() {
	return root;
}

void PawnTree::updateTree() {
	std::vector<std::shared_ptr<Pawn>> pawn_children = root.getChildren();
	for (std::shared_ptr<Pawn> pawn_child : pawn_children) {
		updateTreeRecursion(pawn_child);
	}
}

void PawnTree::updateTreeRecursion(std::shared_ptr<Pawn> pawn_to_update) {
	pawn_to_update->onUpdate();

	//TODO maybe create iterator of some kind with lambda
	std::vector<std::shared_ptr<Pawn>> pawn_children = pawn_to_update->getChildren();
	for (std::shared_ptr<Pawn> pawn_child : pawn_children) {
		updateTreeRecursion(pawn_child);
	}
}

void PawnTree::fixedUpdateTree() {
	std::vector<std::shared_ptr<Pawn>> pawn_children = root.getChildren();
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

std::string PawnTree::print() {
	std::string result = "";
	result += "\"" + root.getName() + "\"" + "\n";
	for (auto c : root.getChildren()) {
		result += recursivePrint(c, 1);
	}
	return result;
}

std::string PawnTree::recursivePrint(std::shared_ptr<Pawn> p, int depth) {
	std::string result = "";
	for (int i = 0; i < depth; i++) {
		result += "  ";
	}
	result += "\"" + p->getName() + "\"" + "\n";

	std::vector<std::shared_ptr<Pawn>> test = p->getChildren();
	for (int i = 0; i < test.size(); i++) {
		result += recursivePrint(test[i], depth + 1);
	}
	return result;
}
