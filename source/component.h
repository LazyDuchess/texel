#pragma once
class Entity;

class Component{
    public:
        Entity* m_owner;
        virtual void OnAdd();
};