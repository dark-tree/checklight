#include "board.hpp"
#include "entity/pawns/spatialPawn.hpp"
#include "shared/logger.hpp"

/*
 * Board
 */


Board::Board() {
    pawns_to_remove = new std::queue<std::shared_ptr<Pawn>>();
    pawns_to_remove_from_hashmap = new std::queue<std::shared_ptr<Pawn>>();
    components_to_remove = new std::queue<std::shared_ptr<Component>>();
}

void Board::queueRemove(const std::shared_ptr<Pawn> &pToRemove) {
#if ENGINE_DEBUG
    if(pToRemove->to_remove && pToRemove->getState() != PawnState::REMOVED){
        FAULT("Pawn unprepared to be removed!");
    }
#endif
    if (pToRemove->is_tracked_on_hash) {
        //pawn needs to be removed from hashmap
        pawns_to_remove->push(pToRemove);
        pawns_to_remove_from_hashmap->push(pToRemove);
    } else {
        pawns_to_remove->push(pToRemove);
    }
}

void Board::queueRemove(const std::shared_ptr<Component> &pToRemove) {
    components_to_remove->push(pToRemove);
}

void Board::updateBoard(double delta) {
    pawns.updateTree(delta);
}


void Board::fixedUpdateBoard() {
    pawns.fixedUpdateTree();
}


void Board::addPawnToRoot(const std::shared_ptr<Pawn> &pawn) {
    pawns.addToRoot(pawn);
    pawn->setBoard(this);
}


std::shared_ptr<Pawn> Board::findPawnByID(uint32_t id) {
    return pawns.findByID(id);
}


std::shared_ptr<Pawn> Board::findPawnByID(uint32_t id, size_t position) {
    std::vector query_result = pawns.findAllByID(id);
    if (query_result.size() > position) {
        return query_result[position];
    }

    FAULT("Trying to get nonexistent Pawn!"); //TODO
}


std::shared_ptr<Pawn> Board::findPawnByID(int32_t id) {
    if (id < 0) {
        FAULT("ID can't be negative!");
    }

    return pawns.findByID((uint32_t) id);
}


std::shared_ptr<Pawn> Board::findPawnByID(int32_t id, size_t position) {
    if (id < 0) {
        FAULT("ID can't be negative!");
    }

    std::vector query_result = pawns.findAllByID(static_cast<uint32_t>(id));
    if (query_result.size() > position) {
        return query_result[position];
    }

    FAULT("Trying to get nonexistent Pawn!"); //TODO
}


std::vector<std::shared_ptr<Pawn>> Board::findPawnsByID(int32_t id) {
    return pawns.findAllByID(id);
}


std::shared_ptr<Pawn> Board::findPawnByName(const std::string &name) {
    return pawns.findByName(name);
}


std::shared_ptr<Pawn> Board::findPawnByName(const std::string &name, size_t position) {
    std::vector query_result = pawns.findAllByName(name);
    if (query_result.size() > position) {
        return query_result[position];
    }

    FAULT("Trying to get nonexistent Pawn!"); //TODO
}


std::vector<std::shared_ptr<Pawn>> Board::findPawnsByName(const std::string &name) {
    return pawns.findAllByName(name);
}


void Board::printBoardTree() {
    out::debug("Entity Tree:\n%s", (pawns.toString() + " ").c_str());
}

void Board::printBoardTreeVerbose() {
    out::debug("Entity Tree:\n%s", (pawns.toStringVerbose() + " ").c_str());
}

void Board::setBoardName(const std::string &new_name) {
    name = new_name;
}


std::string Board::getBoardName() {
    return name;
}

void Board::setCameraPawn(std::shared_ptr<SpatialPawn> &new_cameraPawn) {
    cameraPawn = new_cameraPawn;
}

glm::vec3 Board::getCamPos() {
    return cameraPawn.lock()->getPosition();
}

glm::vec3 Board::getCamForward() {
    return cameraPawn.lock()->getForwardVector();
}

Board::~Board() {
    delete pawns_to_remove;
    delete pawns_to_remove_from_hashmap;
}

void Board::dequeueRemove(size_t amount) {
    while (!pawns_to_remove->empty()) {
        std::shared_ptr<Pawn> *to_be_removed = &pawns_to_remove->front();

        std::destroy((*to_be_removed)->children.begin(), (*to_be_removed)->children.end());

        std::weak_ptr parent = (*to_be_removed)->parent;
        if (!parent.expired()) {
            auto &children = parent.lock()->getChildren();
            children.erase(
                    std::remove_if(children.begin(), children.end(),
                                   [id = (*to_be_removed)->id](const std::shared_ptr<Pawn> &x) {
                                       return x->id == id;
                                   }),
                    children.end()
            );
        }

        (*to_be_removed)->parent.reset();

#if ENGINE_DEBUG
        if(!to_be_removed->get()->is_tracked_on_hash){ //pawn should die here
            std::weak_ptr<Pawn> check = *to_be_removed;
            pawns_to_remove->pop();

            if(!check.expired()){
                FAULT("There is some reference to a pawn, not good...");
            }
        }
        else{
            pawns_to_remove->pop();
        }

#else
        pawns_to_remove->pop();
#endif
    }
        for (size_t i = 0;
             i < (pawns_to_remove_from_hashmap->size() > amount ? amount : pawns_to_remove_from_hashmap->size()); i++) {
            std::shared_ptr<Pawn> *to_be_removed = &pawns_to_remove_from_hashmap->front();

#if ENGINE_DEBUG
            if(!to_be_removed->get()->is_tracked_on_hash){
                FAULT("Trying to remove pawn from hashmap that is explicitly mark for net being in the hashmap");
            }
#endif
            pawns.removeFromMaps((*to_be_removed)->name, (*to_be_removed)->id);

            pawns_to_remove_from_hashmap->pop();
        }
        for (size_t i = 0; i < (components_to_remove->size() > amount ? amount : components_to_remove->size()); i++) {
            std::shared_ptr<Component> *to_be_removed = &components_to_remove->front();

            //todo removing from components hashmap after adding it

            components_to_remove->pop();
        }
    }









