#include "../Entity.h"

class AIController {
    enum ViewDir {
        ViewUp,
        ViewDown,
        ViewLeft,
        ViewRight
    };
    ViewDir _view_dir;
    Entity _entity;
    unsigned int _identifier;
    int _timer;

public:
    AIController(unsigned int identifier, const Entity& entity);
    ~AIController();
    void update(const World& world, std::vector<Entity>& entities);
    const Entity& entity() const;
    unsigned int identifier() const;

};
