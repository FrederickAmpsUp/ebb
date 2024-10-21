use std::{any::{Any, TypeId}, collections::HashMap};

// TODO: more sophisticated (faster) apprach than a hashmap
pub struct Entity {
    components: HashMap<TypeId, Box<dyn Any>>
}

pub trait Component: 'static {}

pub trait System: 'static {
    fn update(&self, world: &mut Vec<Entity>);
}
pub struct World {
    entities: Vec<Entity>,
    systems: Vec<Box<dyn System>>
}

impl Entity {
    pub fn new() -> Self {
        Self { components: HashMap::new() }
    }

    pub fn add_component<T: Component>(&mut self, component: T) {
        let id = TypeId::of::<T>();
        self.components.insert(id, Box::new(component));
    }

    pub fn has_component<T: Component>(&self) -> bool {
        self.components.contains_key(&TypeId::of::<T>())
    }

    pub fn get_component<T: Component>(&self) -> Option<&T> {
        self.components
            .get(&TypeId::of::<T>())
            .and_then(|box_any| box_any.downcast_ref::<T>())
    }
}

impl World {
    pub fn new() -> Self {
        Self {
            entities: vec![],
            systems: vec![]
        }
    }

    pub fn add_entity(&mut self, entity: Entity) {
        self.entities.push(entity);
    }

    pub fn add_system<T: System>(&mut self, system: T) {
        self.systems.push(Box::new(system));
    }

    pub fn get_entities(&self) -> &Vec<Entity> {
        &self.entities
    }

    pub fn update(&mut self) {
        for system in &self.systems {
            system.update(&mut self.entities);
        }
    }
}