#pragma once

#include <unordered_map>
#include <jofilelib.hpp>
#include "Prerequisites.hpp"
#include "Map.hpp"
#include "Object.hpp"

namespace Core {

    class World
    {
    public:
        /// \brief Start with an empty world.
        World();

        /// \brief Get one of the maps by a handle which is equal for server
        ///		and clients.
        Map* GetMap(MapID _id);

        /// \brief Get an object by its unique _id. The id is equal for server
        ///		and clients.
        Object* GetObject(ObjectID _id);

        /// \brief Creates a new map and derives a new id for it.
        ///	\details If a map is created a sync message is sent to the network.
        /// \param [in] _name A name for the new map.
        /// \param [in] _sizeX Initial width of the map. It can be resized
        ///		later.
        ///	\param [in] _sizeY Initial height of the map. It can be resized
        ///		later.
        MapID NewMap(const std::string& _name, unsigned _sizeX, unsigned _sizeY);

        /// \brief Creates a new object and derives a new id for it.
        /// \details The object is not assigned to a world or an inventory, ...
        ///		If an object is created a sync message is sent to the network.
        /// \param [in] Every object must be render able
        ObjectID NewObject( const std::string& _sprite );

        /// \brief Add a new map by deserialization.
        /// \details This method does not send messages to the network.
        MapID NewMap( const Jo::Files::MetaFileWrapper::Node& _node );

        /// \brief Add a new object by deserialization.
        /// \details This method does not send messages to the network.
        ObjectID NewObject( const Jo::Files::MetaFileWrapper::Node& _node );

        /// \brief Add a new object by cloning.
        ObjectID NewObject( const Object* _object );

        /// \brief Delete a map from world.
        /// \details Objects from that map are deleted too!
        /// \param [in] _map ID of the map which has to be deleted.
        void RemoveMap( MapID _map );

        /// \brief Delete an object from world.
        /// \details This cannot remove all references to the object. It is
        ///		assumed that there is no link to that object anymore
        /// \param [in] _object ID of the object which has to be deleted.
        void RemoveObject( ObjectID _object );

        /// \brief Loads all maps and objects of a world from a save-game.
        void Load( Jo::Files::IFile& _file );

        /// \brief Stores all maps and objects to a save game.
        /// TODO: how are players handled?
        void Save( Jo::Files::IFile& _file ) const;

        /// \brief If a player with that name exists its object is returned.
        /// \return The player object or nullptr.
        Object* FindPlayer( std::string _name );

        /// \brief If a player with that ID exists its object is returned.
        /// \return The player object or nullptr.
        Object* FindPlayer( uint8_t _id );

        /// \brief Search all objects which have a certain text sequence in
        ///		their name.
        /// \details This method is case insensitive.
        /// \return An array with read access to all found objects.
        std::vector<ObjectID> FilterObjectsByName( const std::string& _text ) const;

        /// \brief Get all existing map IDs.
        std::vector<MapID> GetAllMaps() const;

        /// \brief Call the update of all maps (Simulate Game).
        /// \param [in] _dt Time since last update.
        void Update( float _dt );
    private:
        /// \brief All real existing objects.
        std::unordered_map<ObjectID, Object> m_objects;
        ObjectID m_nextFreeObjectID;	///< The next unused object handle

        /// \brief A subset of the objects which is flagged with a player property.
        /// \details The objects are mapped to there names to search player by name.
        std::unordered_map<std::string, Core::ObjectID> m_players;

        /// \brief All loaded maps.
        std::unordered_map<MapID, Map> m_maps;
        MapID m_nextFreeMapID;		///< A handle which is not used up to now
    };
} // namespace Core