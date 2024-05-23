#include "Viewer.h"
#include "IC.h"
#include "Layer.h"
#include "MyPolygon.h"
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakePolygon.hxx>
#include <BRepPrimAPI_MakePrism.hxx>
#include <list>


using namespace std;

// polygons
BRepBuilderAPI_MakePolygon make_polygon(list<gp_Pnt> pnts);
list<BRepBuilderAPI_MakePolygon> make_polygons(list<list<gp_Pnt>> pnts);

// faces
BRepBuilderAPI_MakeFace make_face(BRepBuilderAPI_MakePolygon polygon);
list<BRepBuilderAPI_MakeFace> make_faces(list<BRepBuilderAPI_MakePolygon> polygons);

// fuse faces
BRepAlgoAPI_Fuse fuse_faces(list<BRepBuilderAPI_MakeFace> faces);

list<BRepPrimAPI_MakePrism> create_IC(IC ic);


int main(int argc, char** argv)
{
	// create window
	Viewer vout(50, 50, 500, 500);

	// create points
	list<gp_Pnt> pnts{ gp_Pnt(60, -20, 0.0), gp_Pnt(60, -80, 0.0), gp_Pnt(80, -80, 0.0), gp_Pnt(80, -20, 0.0),
					   gp_Pnt(60, -20, 0.0)};
	list<gp_Pnt> pnts2{gp_Pnt(40, -20, 0.0), gp_Pnt(40, -100, 0.0), gp_Pnt(80, -100, 0.0), gp_Pnt(80, -80, 0.0), 
					   gp_Pnt(60, -80, 0.0), gp_Pnt(60, -20, 0.0), gp_Pnt(40, -20, 0.0) };
	list<gp_Pnt> pnts3{ gp_Pnt(56,-20, 0), gp_Pnt(64,-20, 0) , gp_Pnt(64,-28, 0) , gp_Pnt(56,-28, 0),
					   gp_Pnt(56,-20, 0) };

	list<list<gp_Pnt>> points{ pnts, pnts2, pnts3 };

	// create polygons from points
	list<BRepBuilderAPI_MakePolygon> polygons(make_polygons(points));

	// create face from polygons
	list<BRepBuilderAPI_MakeFace> faces = make_faces(polygons);

	// create fused_face from faces
	BRepAlgoAPI_Fuse fused_face = fuse_faces(faces);

	// create prism from fused_face with height z
	double z = 20.0;
	BRepPrimAPI_MakePrism prism(fused_face.Shape(), gp_Vec(0.0, 0.0, z));

	// visualize
	vout << prism;

	vout.StartMessageLoop();
	return 0;
}



BRepBuilderAPI_MakePolygon make_polygon(list<gp_Pnt> pnts)
{
	BRepBuilderAPI_MakePolygon polygon(pnts.front(), *(++pnts.begin()));

	for (auto itr = ++pnts.begin(); itr != pnts.end(); itr++) {
		polygon.Add(*itr);
	}

	return polygon;
}

list<BRepBuilderAPI_MakePolygon> make_polygons(list<list<gp_Pnt>> pnts)
{
	list<BRepBuilderAPI_MakePolygon> polygons;

	for (auto itr = pnts.begin(); itr != pnts.end(); itr++)
	{
		polygons.push_back(make_polygon(*itr));
	}
	return polygons;
}

BRepBuilderAPI_MakeFace make_face(BRepBuilderAPI_MakePolygon polygon)
{
	return BRepBuilderAPI_MakeFace(polygon.Wire(), Standard_True);
}

list<BRepBuilderAPI_MakeFace> make_faces(list<BRepBuilderAPI_MakePolygon> polygons)
{
	list<BRepBuilderAPI_MakeFace> faces;

	for (auto itr = polygons.begin(); itr != polygons.end(); itr++) {
		faces.push_back(make_face(*itr));
	}

	return faces;
}

BRepAlgoAPI_Fuse fuse_faces(list<BRepBuilderAPI_MakeFace> faces)
{
	BRepAlgoAPI_Fuse fused_face(faces.front(), *(++faces.begin()));

	for (auto itr = ++faces.begin(); itr != faces.end(); itr++)
	{
		fused_face = BRepAlgoAPI_Fuse(fused_face.Shape(), (*itr).Shape());
	}
	return fused_face;
}

list<BRepPrimAPI_MakePrism> create_IC(IC ic)
{
	int height = 0;

	list<BRepPrimAPI_MakePrism> prisms{};

	for (vector<Layer>::iterator layer = ic.getLayers().begin(); layer != ic.getLayers().end(); layer++)
	{
		list<list<gp_Pnt>> pnts_of_layer{};
		height += layer->getHeight();

		for (vector<MyPolygon>::iterator polygon = layer->getPolygons().begin(); polygon != layer->getPolygons().end(); polygon++)
		{
			list<gp_Pnt> pnts_of_polygon{};

			for (vector<pair<int, int>>::iterator point = polygon->getCoordinates().begin(); point != polygon->getCoordinates().end(); point++)
			{
				gp_Pnt pnt(point->first, point->second, height);
				pnts_of_polygon.push_back(pnt);
			}

			pnts_of_layer.push_back(pnts_of_polygon);
		}

		list<BRepBuilderAPI_MakePolygon> polygons = make_polygons(pnts_of_layer);
		list<BRepBuilderAPI_MakeFace> faces = make_faces(polygons);
		BRepAlgoAPI_Fuse fused_face = fuse_faces(faces);
		BRepPrimAPI_MakePrism prism(fused_face.Shape(), gp_Vec(0.0, 0.0, layer->getThickness()));
		prisms.push_back(prism);
	}

	return prisms;
}