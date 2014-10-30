#ifndef __GEODESIC_H__
#define __GEODESIC_H__

#include "PhyloTree.h"
#include "PhyloTreeEdge.h"
#include "RatioSequence.h"
#include <string>
#include <vector>

using namespace std;

class Geodesic {
public:
    Geodesic(RatioSequence rs);

    Geodesic(RatioSequence rs, vector<PhyloTreeEdge> cEdges);

    Geodesic(RatioSequence rs, vector<PhyloTreeEdge> cEdges, double leafContributionSquared);

    Geodesic(const Geodesic& other);

    PhyloTree getTreeAt(PhyloTree t1, PhyloTree t2, double position);

    RatioSequence getRS();

    void setRS(RatioSequence rs);

    double getDist();

    void addCommonEdge(PhyloTreeEdge e);

    Geodesic clone();

    string toString();

    static vector<PhyloTreeEdge> getCommonEdges(PhyloTree t1, PhyloTree t2, double position);

    vector<PhyloTreeEdge> getCommonEdges();

    void setCommonEdges(vector<PhyloTreeEdge> commonEdges);

    size_t numCommonEdges();

    size_t numTopologies();

    Geodesic reverse();

    double getLeafContributionSquared();

    void setLeafContributionSquared(double leafContributionSquared);

    static Geodesic getGeodesic(PhyloTree t1, PhyloTree t2);

    static Geodesic getGeodesicNoCommonEdges(PhyloTree t1, PhyloTree t2);

private:
    RatioSequence rs;
    vector<PhyloTreeEdge> commonEdges;
    double leafContributionSquared = 0;
    static void splitOnCommonEdge(PhyloTree &t1, PhyloTree &t2, vector<PhyloTree> &destination_a, vector<PhyloTree> &destination_b);
};

#endif /* __GEODESIC_H__ */
