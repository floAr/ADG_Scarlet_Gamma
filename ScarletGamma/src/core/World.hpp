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

		/// \brief Create the default object template bases
		void Init();

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
		/// \param [in] _newID Give the map the next free id instead of
		///		using the serialized id. This is useful for imports.. where
		///		ids must be rescheduled.
        MapID NewMap( Jo::Files::MetaFileWrapper::Node& _node, bool _newID );

        /// \brief Add a new object by deserialization.
        /// \details This method does not send messages to the network.
        /// \param [in] _newID Give the object the next free id instead of
		///		using the serialized id. This is useful for imports.. where
		///		ids must be rescheduled.
        ObjectID NewObject( Jo::Files::MetaFileWrapper::Node& _node, bool _newID );

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
        Object* FindPlayer( Core::PlayerID _id );

        /// \brief Search all objects which have a certain text sequence in
        ///		their name.
        /// \details This method is case insensitive.
        /// \return An array with read access to all found objects.
        std::vector<ObjectID> FilterObjectsByName( const std::string& _text ) const;

        /// \brief Get all existing map IDs.
        std::vector<MapID> GetAllMaps() const;

		/// \brief Get all existing player's object IDs.
		const std::vector<ObjectID>& GetAllPlayers() const;

        /// \brief Call the update of all maps (Simulate Game).
        /// \param [in] _dt Time since last update.
        void Update( float _dt );

		/// \brief Get the id of the object which contains all (template) properties.
		Object* GetPropertyBaseObject()	{ return GetObject(m_propertyTemplates); }

		const ObjectList& GetModuleBase()	{ return m_moduleTemplates; }
		const ObjectList& GetTemplateBase()	{ return m_objectTemplates; }
		ObjectID NewModuleTemplate( const std::string& _sprite );
		ObjectID NewObjectTemplate( const std::string& _sprite );
		ObjectID NewObjectTemplate( ObjectID _object );

		/// \brief Returns the next observable object
		/// \param [in] _currentID ObjectID of the object currently watched
		/// \param [in] _direction -1 to get the previous one and +1 to get
		///			the next one.
		Object* GetNextObservableObject(ObjectID _currentID, int _direction);

		/// \brief Registers an object in the corresponding list (currently player and owner)
		/// \param [in] _object Object to be registered
		void RegisterObject(Object* _object);

		/// \brief Remove an object from all lists
		/// \param [in] _object ID of the object which should be checked.
		void UnregisterObject(ObjectID _object);

		/// \brief Checks the special pause-object-property if it is set to true.
		bool IsPaused();
		/// \brief Freeze or continue game.
		void SetPause( bool _pause );

		/// \brief Write a single map and all referenced objects into an extra file.
		/// \param [in] _file An opened file with write access.
		/// \param [in] _map The map which should be exported.
		void ExportMap( Jo::Files::IFile& _file, MapID _map ) const;

		/// \brief Imports everything from another world file.
		/// \param [in] _file An opened file with read access containing a
		///		world in metafile format.
		void Import( Jo::Files::IFile& _file );
    private:
        /// \brief All real existing objects.
        std::unordered_map<ObjectID, Object> m_objects;
        ObjectID m_nextFreeObjectID;	///< The next unused object handle

        /// \brief A subset of the objects which is flagged with a player property.
        /// \details The objects are mapped to there names to search player by name.
        std::vector<Core::ObjectID> m_players;

		/// \brief A subset of the objects which is flagged with the owner property.
        /// \details The objects are mapped to there names to search this objects by name.
		std::vector<Core::ObjectID> m_ownedObjects;

		/// \brief A subset of objects which define templates
		ObjectList m_objectTemplates;
		/// \brief Creates the initial template database in case of a
		///		loading failure.
		void CreateDefaultTemplateBase();
		/// \brief A subset of objects which define modules
		ObjectList m_moduleTemplates;
		/// \brief Creates the initial module database in case of a
		///		loading failure.
		void CreateDefaultModuleBase();
		/// \brief One object with all properties
		Core::ObjectID m_propertyTemplates;
		/// \brief Creates the initial property database in case of a
		///		loading failure.
		void CreateDefaultPropertyBase();

        /// \brief All loaded maps.
        std::unordered_map<MapID, Map> m_maps;
        MapID m_nextFreeMapID;		///< A handle which is not used up to now
    };
} // namespace Core