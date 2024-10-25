use std::{any::{Any, TypeId}, collections::HashMap};

// TODO: more sophisticated (faster) apprach than a hashmap

/// A collection of components, to be stored in a [World].
pub struct Entity {
    components: HashMap<TypeId, Box<dyn Any>>
}

/// An object, to be attached to an [Entity], which stores some arbitrary data.
pub trait Component: 'static {}

/// Some functionality which operates on [Entity]s that have some specific [Component]\(s).
pub trait System: 'static {
    /// Run the system's functionality.
    /// 
    /// # Arguments
    /// 
    /// * `world` - a [Vec] of [Entity]s. The [System] is responsible for finding the ones with the desired [Component]\(s).
    fn update(&self, world: &mut Vec<Entity>);
}

/// A collection of [Entity]s and [System]s.
pub struct World {
    entities: Vec<Entity>,
    systems: Vec<Box<dyn System>>
}

impl Entity {
    /// Creates a new [Entity] with no [Component]s.
    /// 
    /// # Returns
    /// 
    /// The new, empty [Entity].
    pub fn new() -> Self {
        Self { components: HashMap::new() }
    }

    /// Add a [Component] to this [Entity].
    /// 
    /// # Type Parameters
    /// 
    /// * `T` - a [Component], the type of the argument.
    /// 
    /// # Arguments
    /// 
    /// * `component` - The component to add.
    pub fn add_component<T: Component>(&mut self, component: T) {
        let id = TypeId::of::<T>();
        self.components.insert(id, Box::new(component));
    }

    /// Check if this [Entity] has a specified [Component] attached.
    /// 
    /// # Type Parameters
    /// 
    /// * `T` - a [Component], the type to check.
    /// 
    /// # Returns
    /// 
    /// Wether or not this [Entity] has the specified [Component] attached.
    pub fn has_component<T: Component>(&self) -> bool {
        self.components.contains_key(&TypeId::of::<T>())
    }

    /// Get a specified [Component] from this [Entity]
    /// 
    /// # Type Parameters
    /// 
    /// * `T` - a [Component], the type to return.
    /// 
    /// # Returns
    /// 
    /// `Some(component)` if this [Entity] contains the specified [Component].  
    /// `None` otherwise.
    pub fn get_component<T: Component>(&self) -> Option<&T> {
        self.components
            .get(&TypeId::of::<T>())
            .and_then(|box_any| box_any.downcast_ref::<T>())
    }
}

// TODO: a way to search through the entities to find ones with a specific component (fast)
impl World {

    /// Create a new [World] with no [Entity]s or [System]s.
    /// 
    /// # Returns
    /// 
    /// The empty [World].
    pub fn new() -> Self {
        Self {
            entities: vec![],
            systems: vec![]
        }
    }

    /// Add an [Entity] to this [World].
    /// 
    /// # Arguments
    /// 
    /// * `entity` - the [Entity] to add.
    pub fn add_entity(&mut self, entity: Entity) {
        self.entities.push(entity);
    }

    /// Add a [System] to this [World].
    /// 
    /// # Type Parameters
    /// 
    /// * `T` - the type of the [System] to add.
    /// 
    /// # Arguments
    /// 
    /// * `system` - the [System] to add.
    pub fn add_system<T: System>(&mut self, system: T) {
        self.systems.push(Box::new(system));
    }

    /// Get all the [Entity]s from this [World].
    /// 
    /// # Returns
    /// 
    /// A reference to the internal [Vec] of [Entity]s.
    pub fn get_entities(&self) -> &Vec<Entity> {
        &self.entities
    }

    /// Update all [System]s in this [World].
    pub fn update(&mut self) {
        for system in &self.systems {
            system.update(&mut self.entities);
        }
    }
}