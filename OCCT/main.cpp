#include "Viewer.h"
#include "IC.h"
#include "Layer.h"
#include "MyPolygon.h"
#include "ICFactory.h"

#include <list>
#include <vector>

#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakePolygon.hxx>
#include <BRepPrimAPI_MakePrism.hxx>
#include <BRepOffsetAPI_MakeOffset.hxx>

#include <STEPControl_Writer.hxx>
#include <STEPCAFControl_Writer.hxx>
#include <TDF_Label.hxx>
#include <Quantity_Color.hxx>
#include <BRep_Builder.hxx>
#include <TCollection_ExtendedString.hxx>

#include <TDocStd_Application.hxx>
#include <BinXCAFDrivers.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_ColorTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <TDataStd_Name.hxx>

#include <Geom_Plane.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>

// polygons
BRepBuilderAPI_MakePolygon make_polygon(std::list<gp_Pnt>& pnts);
std::list<BRepBuilderAPI_MakePolygon> make_polygons(std::list<std::list<gp_Pnt>>& pnts);

// faces
BRepBuilderAPI_MakeFace make_face(BRepBuilderAPI_MakePolygon& polygon);
std::list<BRepBuilderAPI_MakeFace> make_faces(std::list<BRepBuilderAPI_MakePolygon>& polygons);

// fuse faces
BRepAlgoAPI_Fuse fuse_faces(std::list<BRepBuilderAPI_MakeFace>& faces);

// offset
TopoDS_Shape ConvertCurvesToLines(const TopoDS_Shape& shape);
TopoDS_Shape create_offset(std::vector<std::pair<int, int>> points, int width, int path_type);

void create_IC(IC ic, Handle(XCAFDoc_ShapeTool)& shape_tool, Handle(XCAFDoc_ColorTool)& color_tool, Viewer& vout);



const std::vector<Quantity_Color> COLORS = {
		Quantity_Color(Quantity_NOC_RED),
		Quantity_Color(Quantity_NOC_GREEN),
		Quantity_Color(Quantity_NOC_BLUE),
		Quantity_Color(Quantity_NOC_YELLOW),
		Quantity_Color(Quantity_NOC_CYAN),
		Quantity_Color(Quantity_NOC_MAGENTA)
};



bool WriteSTEP(const Handle(TDocStd_Document)& doc, const char* filename)
{
	STEPCAFControl_Writer Writer;
	if (!Writer.Transfer(doc))
		return false;
	if (Writer.Write(filename) != IFSelect_RetDone)
		return false;

	return true;
}

int main(int argc, char** argv)
{
	Handle(TDocStd_Application) app = new TDocStd_Application;
	BinXCAFDrivers::DefineFormat(app);
	Handle(TDocStd_Document) doc;
	app->NewDocument("BinXCAF", doc);

	Handle(XCAFDoc_ShapeTool) shape_tool = XCAFDoc_DocumentTool::ShapeTool(doc->Main());
	Handle(XCAFDoc_ColorTool) color_tool = XCAFDoc_DocumentTool::ColorTool(doc->Main());

	// create window
	Viewer vout(50, 50, 500, 500);

	// create 3D-IC from gdsII file 
	std::string path = R"(C:\Users\Alici\Documents\Projekt Digitalisierung - Digital Twins\repository\Projekt-Digital-Twin-Model-Convertion\example_files\SDFFRS_X2.gds)";
	std::string stackUp = R"(C:\Users\Alici\Documents\Projekt Digitalisierung - Digital Twins\repository\Projekt-Digital-Twin-Model-Convertion\example_files\SDFFRS_X2.CSV)";
	IC new_IC = ICFactory::generateIC("Test", path, stackUp);
	create_IC(new_IC, shape_tool, color_tool, vout);

	// write XDE file into STEP file
	WriteSTEP(doc, "test.stp");

	// test for offset line
	/*std::vector<std::pair<int, int>> v;
	v.emplace_back(std::make_pair(100, 200));
	v.emplace_back(std::make_pair(200, 200));
	v.emplace_back(std::make_pair(200, 300));
	v.emplace_back(std::make_pair(250, 300));
	BRepBuilderAPI_MakePolygon mkPolygon;
	for (const auto& point : v)
	{
		gp_Pnt pt(point.first, point.second, 0);
		mkPolygon.Add(pt);
	}
	vout << mkPolygon.Shape();

	vout << create_offset(v, 50, 1);*/

	vout.StartMessageLoop();
	return 0;
}



BRepBuilderAPI_MakePolygon make_polygon(std::list<gp_Pnt>& pnts)
{
	BRepBuilderAPI_MakePolygon polygon;

	for (auto itr: pnts) {
		polygon.Add(itr);
	}

	return polygon;
}

std::list<BRepBuilderAPI_MakePolygon> make_polygons(std::list<std::list<gp_Pnt>>& pnts)
{
	std::list<BRepBuilderAPI_MakePolygon> polygons;

	for (auto itr: pnts)
	{
		polygons.push_back(make_polygon(itr));
	}
	return polygons;
}

BRepBuilderAPI_MakeFace make_face(BRepBuilderAPI_MakePolygon& polygon)
{
	return BRepBuilderAPI_MakeFace(polygon.Wire(), Standard_True);
}

std::list<BRepBuilderAPI_MakeFace> make_faces(std::list<BRepBuilderAPI_MakePolygon>& polygons)
{
	std::list<BRepBuilderAPI_MakeFace> faces;

	for (auto itr: polygons) {
		faces.push_back(make_face(itr));
	}

	return faces;
}

BRepAlgoAPI_Fuse fuse_faces(std::list<BRepBuilderAPI_MakeFace>& faces)
{
	BRepAlgoAPI_Fuse fused_face(faces.front().Shape(), (++faces.begin())->Shape());

	int i = 0;
	for (auto itr: faces)
	{
		if (i >= 2)
		{
			fused_face = BRepAlgoAPI_Fuse(fused_face.Shape(), itr.Shape());
		}
		i++;
	}
	return fused_face;
}

TopoDS_Shape ConvertCurvesToLines(const TopoDS_Shape& shape) {
	BRepBuilderAPI_MakeWire wireBuilder;

	for (TopExp_Explorer expEdge(shape, TopAbs_EDGE); expEdge.More(); expEdge.Next()) {
		TopoDS_Edge edge = TopoDS::Edge(expEdge.Current());
		TopoDS_Vertex v1, v2;
		TopExp::Vertices(edge, v1, v2);
		gp_Pnt p1 = BRep_Tool::Pnt(v1);
		gp_Pnt p2 = BRep_Tool::Pnt(v2);
		wireBuilder.Add(BRepBuilderAPI_MakeEdge(p1, p2));
	}

	return wireBuilder.Shape();
}

// path type:
// 0: square end, flush
// 1: rounded end
// 2: square end, extend width/2
TopoDS_Shape create_offset(std::vector<std::pair<int, int>> points, int width, int path_type)
{
	Handle(Geom_Plane) pln = new Geom_Plane(gp::XOY()); //TODO: plane from layer information
	TopoDS_Shape shape; //TODO: delete when finished writing the function

	BRepBuilderAPI_MakePolygon mkPolygon;
	for (const auto& point : points)
	{
		mkPolygon.Add(gp_Pnt(point.first, point.second, 0)); //TODO: add height of layer
	}
	TopoDS_Face f = BRepBuilderAPI_MakeFace(pln, mkPolygon.Wire(), false);

	switch (path_type)
	{
	case 0:
	{

		return shape;
	}
	case 1:
	{
		BRepOffsetAPI_MakeOffset mkOffset(f, GeomAbs_Intersection, false);
		Standard_Real offset = width / 2.0;
		mkOffset.Perform(offset);
		return mkOffset.Shape();
	}

	case 2:
	{
		BRepOffsetAPI_MakeOffset mkOffset(f, GeomAbs_Intersection, false);
		Standard_Real offset = width / 2.0;
		mkOffset.Perform(offset);
		shape = mkOffset.Shape();
		shape = ConvertCurvesToLines(mkOffset.Shape());
		return shape;
	}
	}

	return {};
}

void create_IC(IC ic, Handle(XCAFDoc_ShapeTool)& shape_tool, Handle(XCAFDoc_ColorTool)& color_tool, Viewer& vout)
{
	float height = 0;
	int color_index = 0;

	for (auto layer : ic.getLayers())
	{
		std::list<std::list<gp_Pnt>> pnts_of_layer{};
		height += layer.getHeight();

		for (auto polygon : layer.getPolygons())
		{
			std::list<gp_Pnt> pnts_of_polygon{};

			for (auto point : polygon.getCoordinates())
			{
				gp_Pnt pnt(point.first, point.second, height);
				pnts_of_polygon.push_back(pnt);
			}
			pnts_of_layer.push_back(pnts_of_polygon);
		}

		auto polygons = make_polygons(pnts_of_layer);
		auto faces = make_faces(polygons);
		BRepAlgoAPI_Fuse* fused_face = new BRepAlgoAPI_Fuse(fuse_faces(faces));
		BRepPrimAPI_MakePrism prism(fused_face->Shape(), gp_Vec(0.0, 0.0, layer.getThikness()));

		// add prism to XDE file
		TDF_Label label = shape_tool->AddShape(prism, false);
		color_tool->SetColor(label, COLORS[color_index++ % COLORS.size()], XCAFDoc_ColorGen);

		std::string name = "layer" + std::to_string(layer.getId());
		TDataStd_Name::Set(label, name.c_str());

		// visualize
		vout << prism;
	}
}

