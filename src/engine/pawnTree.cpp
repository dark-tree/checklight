#include "pawnTree.hpp"
#include "shared/logger.hpp"

/*
 * PawnTree
 */

PawnTree::PawnTree() {
	root = std::make_shared<RootPawn>();
	//TODO adding root to hashmap (?)
	root->pawn_state = PawnState::TRACKED;
	root->tr = this;
}

std::shared_ptr<Pawn> PawnTree::findByName(const std::string& name) {
	std::unordered_multimap<std::string, std::shared_ptr<Pawn>>::iterator result = name_map.find(name);
	if (result != name_map.end()) {
		return result->second;
	}
	return nullptr;
}

std::shared_ptr<Pawn> PawnTree::findByID(uint32_t id) {
	std::unordered_multimap<uint32_t, std::shared_ptr<Pawn>>::iterator result = id_map.find(id);
	if (result != id_map.end()) {
		return result->second;
	}
	return nullptr;
}

bool PawnTree::removeFromMaps(const std::string& name, uint32_t id) {
	int erasedAmount = 0;

	auto results = name_map.equal_range(name);

	for (auto it = results.first; it != results.second;) {
		if (it->second->id == id) {
			it = name_map.erase(it);
			erasedAmount++;
			continue;
		}
		++it;
	}

	if (erasedAmount == 1) {
	} else {
		FAULT("There should be only one entity with given name and ID");
	}

	id_map.erase(id_map.find(id));

	return true;
}

std::vector<std::shared_ptr<Pawn>> PawnTree::findAllByName(const std::string& name) {
	std::vector<std::shared_ptr<Pawn>> result;
	auto range = name_map.equal_range(name);
	for (auto r = range.first; r != range.second; ++r) {
		result.push_back(r->second);
	}
	return result;
}

std::vector<std::shared_ptr<Pawn>> PawnTree::findAllByID(const uint32_t id) {
	std::vector<std::shared_ptr<Pawn>> result;
	auto range = id_map.equal_range(id);
	for (auto r = range.first; r != range.second; ++r) {
		result.push_back(r->second);
	}
	return result;
}

size_t PawnTree::nameHitSize(const std::string& name) const {
	return name_map.count(name);
}

size_t PawnTree::idHitSize(uint32_t id) const {
	return id_map.count(id);
}

void PawnTree::addToRoot(const std::shared_ptr<Pawn>& pawn) {
	root->addChild(pawn);
	mountPawn(pawn);
}

void PawnTree::mountPawn(const std::shared_ptr<Pawn>& pawn) {
	bool isCopy = false;
	bool isChanged = pawn->unregisteredChildAdded();

	if (id_map.count(pawn->getEntityID()) > 0) {
		auto range = id_map.equal_range(pawn->getEntityID());
		for (auto r = range.first; r != range.second; ++r) {
			if (r->second == pawn) {
				isCopy = true;
			}
		}
	}

	if (!pawn->physics_component.expired()) {
		physics_components_to_update.insert(pawn->physics_component.lock());
	}

	//TODO test if it works
	if (!isCopy) {
		const std::string p_name = pawn->getName();
		const uint32_t p_id = pawn->getEntityID();
		addPawnToHash(p_name, p_id, pawn);
	}

	if (isChanged) {
		updatePawnsChildren(pawn);
		isChanged = false;
	}
}

void PawnTree::updatePawnsChildren(const std::shared_ptr<Pawn>& pawn) {
	for (std::shared_ptr<Pawn>& pawn_child: pawn->getChildren()) {
		mountPawn(pawn_child);
	}
}

std::shared_ptr<RootPawn> PawnTree::getRoot() {
	return root;
}

void PawnTree::updateTree(double delta, std::mutex& mtx) {
	for (std::shared_ptr<Pawn>& pawn_child: root->getChildren()) {
		updateTreeRecursion(pawn_child, delta, mtx);
	}
}

void PawnTree::updateTreeRecursion(std::shared_ptr<Pawn> pawn_to_update, double delta, std::mutex& mtx) {
	mtx.lock();
	pawn_to_update->onUpdate(delta);
	mtx.unlock();

	//TODO maybe create iterator of some kind with lambda
	for (std::shared_ptr<Pawn>& pawn_child: pawn_to_update->getChildren()) {
		updateTreeRecursion(pawn_child, delta, mtx);
	}
}

void PawnTree::fixedUpdateTree() {
	for (std::shared_ptr<Pawn>& pawn_child: root->getChildren()) {
		fixedUpdateTreeRecursion(pawn_child);
	}
}

void PawnTree::fixedUpdateTreeRecursion(std::shared_ptr<Pawn> pawn_to_fixed_update) {
	pawn_to_fixed_update->onFixedUpdate();

	//TODO maybe create iterator of some kind with lambda
	for (std::shared_ptr<Pawn>& pawn_child: pawn_to_fixed_update->getChildren()) {
		fixedUpdateTreeRecursion(pawn_child);
	}
}

std::string PawnTree::toString() {
	return printStart(false);
}


std::string PawnTree::toStringVerbose() {
	return printStart(true);
}

void PawnTree::registerPhysicsComponent(const std::shared_ptr<PhysicsComponent>& physics_component) {
	physics_components_to_update.insert(physics_component);
}

void PawnTree::removePhysicsComponent(const std::shared_ptr<PhysicsComponent>& physics_component) {
	physics_components_to_update.erase(physics_component);
}

std::string PawnTree::printStart(bool verbose) {
	std::string result;
	std::string n;
	if (!verbose) n = root->toString();
	else n = root->toStringVerbose();
	result += "Root:" + n + "\n";
	for (const auto& c: root->getChildren()) {
		result += recursiveString(c, 1, verbose);
	}
	return result;
}


std::string PawnTree::recursiveString(const std::shared_ptr<Pawn>& p, const int depth, const bool verbose) {
	std::string result;
	for (int i = 0; i < depth; i++) {
		result += "  ";
	}

	std::string n;
	if (!verbose) n = p->toString();
	else n = p->toStringVerbose();

	result += "Pawn:" + n + "\n";

	if (verbose) {
		for (const auto& component: p->components) {
			for (int i = 0; i <= depth; i++) {
				result += "  ";
			}
			result += "Component:" + component->toString() + "\n";
		}
	}

	std::vector<std::shared_ptr<Pawn>> p_children = p->getChildren();
	for (const auto& i: p_children) {
		result += recursiveString(i, depth + 1, verbose);
	}
	return result;
}

void PawnTree::addPawnToHash(const std::string& p_name, uint32_t p_id, const std::shared_ptr<Pawn>& pawn) {
	name_map.insert(std::pair<std::string, std::shared_ptr<Pawn>>(p_name, pawn));
	id_map.insert(std::pair<uint32_t, std::shared_ptr<Pawn>>(p_id, pawn));
	pawn->setTracked(true);
}

std::set<std::shared_ptr<PhysicsComponent>> PawnTree::getPhysicsComponents() {
	return physics_components_to_update;
}
