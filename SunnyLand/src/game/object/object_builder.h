#pragma once
#include "../../engine/object/object_builder.h"

namespace game::object
{
    class ObjectBuilderSL final : public engine::object::ObjectBuilder
    {
    public:
        ObjectBuilderSL(engine::core::Context &context, engine::scene::LevelLoader &level_loader);
        ~ObjectBuilderSL();

        void build() override;

    protected:
        void buildPlayer();
        void buildEnemy();
        void buildItem();
    };
}
