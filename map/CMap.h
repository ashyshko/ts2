#pragma once

#include <map>
#include <set>
#include <vector>

#include <Defs.h>
#include <CWayPoint.h>
#include <CPath.h>
#include <CJunction.h>
#include <CVehicle.h>

class CMap
{
public:
    explicit CMap()
    {

    }

    ~CMap()
    {

    }

    WayPointId_t AddWayPoint()
    {
        CWayPoint* wp = new CWayPoint;
        WayPointId_t wp_id = wp->WayPointId();
        assert( m_wps.count(wp_id) == 0 );
        m_wps.insert(wp_id);
        return wp_id;
    }

    void RemoveWayPoint( WayPointId_t wp_id )
    {
        assert( m_wps.count(wp_id) == 1 );
        CWayPoint* wp = ToWayPoint(wp_id);
        m_wps.erase(wp_id);
        delete wp;
    }

    PathId_t AddPath( WayPointId_t begin_wp, WayPointId_t end_wp, Distance_t distance )
    {
        assert( m_wps.count(begin_wp) == 1 );
        assert( m_wps.count(end_wp) == 1 );
        CPath* path = new CPath( begin_wp, end_wp, distance );
        PathId_t path_id = path->PathId();
        assert( m_pathes.count(path_id) == 0 );
        m_pathes.insert(path_id);
        return path_id;
    }

    void RemovePath( PathId_t path_id )
    {
        assert( m_pathes.count(path_id) == 1 );
        CPath* path = ToPath(path_id);
        m_pathes.erase(path_id);
        delete path;
    }

    JunctionId_t CreateJunction( PathId_t hi_path, Distance_t hi_path_begin, Distance_t hi_path_end, PathId_t lo_path, Distance_t lo_path_begin, Distance_t lo_path_end )
    {
        assert( m_pathes.count(hi_path) == 1 );
        assert( m_pathes.count(lo_path) == 1 );
        CJunction* junc = new CJunction( hi_path, hi_path_begin, hi_path_end, lo_path, lo_path_begin, lo_path_end );
        JunctionId_t junc_id = junc->JunctionId();
        assert( m_juncs.count(junc_id) == 0 );
        m_juncs.insert(junc_id);
        return junc_id;
    }

    void RemoveJunction( JunctionId_t junc_id )
    {
        assert( m_juncs.count(junc_id) == 1 );
        CJunction* junc = ToJunction(junc_id);
        m_juncs.erase(junc_id);
        delete junc;
    }

    VehicleId_t AddVehicle( EVehicleClass veh_class, PathId_t path, Distance_t path_pos )
    {
        assert( m_pathes.count(path) == 1 );
        CVehicle* veh = new CVehicle( veh_class, path, path_pos, std::vector<Distance_t>(), s_vehicle_dimensions[veh_class].length );
        VehicleId_t veh_id = veh->VehicleId();
        assert( m_vehs.count(veh_id) == 0 );
        m_vehs.insert(veh_id);
        return veh_id;
    }

    bool SetVehicleDestination( VehicleId_t veh_id, WayPointId_t wp )
    {
        assert( m_vehs.count(veh_id) == 0 );
        CVehicle* veh = ToVehicle(veh_id);

        PathId_t current_path = veh->GetCurrentPath();

        std::vector<PathId_t> new_route;
        if( !CalculateRoute( ToPath( current_path )->end_wp, wp, new_route ) )
        {
            return false;
        }

        veh->ReplaceRoute( std::move(new_route) );
        return true;
    }

    void RemoveVehicle( VehicleId_t veh_id )
    {
        assert( m_vehs.count(veh_id) == 0 );
        CVehicle* veh = ToVehicle(veh_id);
        m_vehs.erase(veh_id);
        delete veh;
    }


private:
    std::set<WayPointId_t> m_wps;
    std::set<PathId_t> m_pathes;
    std::set<JunctionId_t> m_juncs;
    std::set<VehicleId_t> m_vehs;

};