#This file is auto generated by the PythonDocGenerator tool. Please do not edit manually.
#Its sole purpose is to provide Python stubs for the C++ code, which can be used in your IDE to help during development.
#Do note that these types aren't complete, with constructor args and properties missing.
"""Python bindings for code related to the simulation of the world."""

OPERATION_BLOCKED = 2
OPERATION_HANDLED = 1
OPERATION_IGNORED = 0
def get_alias_entity():
    """Gets an entity registered by an alias, if available."""
    pass
class Domain:
    def __init__(self, *args, **kwargs):
        pass
    def query_collisions(self):
        pass

class EntityProps:
    def __init__(self, *args, **kwargs):
        pass

class Task:
    def __init__(self, *args, **kwargs):
        pass
    def get_arg(self):
        pass
    def irrelevant(self):
        pass
    def obsolete(self):
        pass
    def start_action(self):
        """Starts an action that will be tied to this task."""
        pass
    def stop_action(self):
        """Stops an action that previously was started."""
        pass

class TerrainProperty:
    def __init__(self, *args, **kwargs):
        pass
    def find_mods(self):
        pass
    def get_height(self):
        pass
    def get_normal(self):
        pass
    def get_surface(self):
        """Gets the numerical index of the surface that's most prevalent at the location.
Parameters:
x : The x position
y : The y position"""
        pass
    def get_surface_name(self):
        """Gets the name of the surface that's most prevalent at the location.
Parameters:
x : The x position
y : The y position"""
        pass

class Thing:
    def __init__(self, *args, **kwargs):
        pass
    def as_entity(self):
        pass
    def can_reach(self):
        pass
    def client_error(self):
        pass
    def create_new_entity(self):
        """Creates a new entity."""
        pass
    def describe_entity(self):
        pass
    def find_in_contains(self):
        """Returns a list of all contained entities that matches the supplied Entity Filter."""
        pass
    def get_child(self):
        pass
    def get_parent_domain(self):
        """Gets the parent domain, i.e. the domain to which this entity belongs."""
        pass
    def get_prop_bool(self):
        pass
    def get_prop_float(self):
        pass
    def get_prop_int(self):
        pass
    def get_prop_list(self):
        pass
    def get_prop_map(self):
        pass
    def get_prop_num(self):
        pass
    def get_prop_string(self):
        pass
    def has_prop_bool(self):
        pass
    def has_prop_float(self):
        pass
    def has_prop_int(self):
        pass
    def has_prop_list(self):
        pass
    def has_prop_map(self):
        pass
    def has_prop_num(self):
        pass
    def has_prop_string(self):
        pass
    def is_type(self):
        pass
    def mod_property(self):
        pass
    def send_world(self):
        pass
    def start_action(self):
        """Starts a new action. First parameter is the action name, and the second is the duration. Since it's not tied to a task a duration is required."""
        pass
    def start_task(self):
        pass
    def update_task(self):
        pass

class Usage:
    def __init__(self, *args, **kwargs):
        pass

class UsageInstance:
    def __init__(self, *args, **kwargs):
        pass
    def get_arg(self):
        pass
    def is_valid(self):
        pass

class World:
    def __init__(self, *args, **kwargs):
        pass
    def get_entity(self):
        """Gets the entity with the supplied id."""
        pass
    def get_time(self):
        """Get server time in milliseconds."""
        pass
    def get_time_as_seconds(self):
        pass
    def match_entity(self):
        """Matches a filter against an entity."""
        pass

