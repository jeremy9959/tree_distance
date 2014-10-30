#include "PhyloTree.h"
//#include "Tools.h"
#include <cmath>

#define LENGTH_DEFAULT 0.0

using namespace std;

PhyloTree::PhyloTree(vector<PhyloTreeEdge> edges, vector<string> leaf2NumMap, vector<EdgeAttribute> leafEdgeLengths) {
    this->edges = edges;
    this->leaf2NumMap = leaf2NumMap;
    this->leafEdgeAttribs = leafEdgeLengths;
}

PhyloTree::PhyloTree(vector<PhyloTreeEdge> edges, vector<string> leaf2NumMap) {
    this->leaf2NumMap = leaf2NumMap;
    size_t len = leaf2NumMap.size();
    for (auto &edge : edges) {
        boost::dynamic_bitset<> new_bitset(len);
        auto partition = *(edge.getPartition());
        for (size_t i=0; i < len; ++i) {
            new_bitset[len - i - 1] = partition[partition.size() - i - 1];
        }
        edge.setOriginalEdge(Bipartition(new_bitset));
        this->edges.push_back(edge);
    }
}

PhyloTree::PhyloTree(const PhyloTree &t) {
    edges = vector<PhyloTreeEdge>(t.edges);
    leaf2NumMap = vector<string>(t.leaf2NumMap);
    leafEdgeAttribs = vector<EdgeAttribute>(t.leafEdgeAttribs);
    newick = string(t.newick);
}

PhyloTree::PhyloTree(string t, bool rooted) {
    int leafNum = 0;
    deque<PhyloTreeEdge> q;
    string label, length;
    size_t i = 0;

    // remove anything before the first (
    t = t.substr(t.find_first_of("("));
    newick = t;

    // remove whitespace
    vector<string> split_newick = Tools::string_split(newick, " \t\n", "");
    t = Tools::string_join(split_newick, "");

    //pull off ';' if at end
    t = t.erase(t.find_last_of(";"));

    // pull off the first and last brackets (and a root length, between the last bracket and ;, if there is one.
    t = t.substr(t.find_first_of("(") + 1);
    t = t.erase(t.find_last_of(")"));

    // do bracket counting sanity check
    if (count(t.begin(), t.end(), '(') != count(t.begin(), t.end(), ')')) {
        throw invalid_argument("Bracket mismatch error in tree: " + t);
    }

    try {
        size_t end_of_label, end_of_length, alt_end_of_label;
        setLeaf2NumMapFromNewick();
        leafEdgeAttribs = vector<EdgeAttribute>(leaf2NumMap.size());
        while (i < t.size()) {
            switch (t.at(i)) {
                case '(': {
                    q.push_front(PhyloTreeEdge(string(leaf2NumMap.size(), '0')));
                    i++;
                    break;
                }

                case ')': {
                    end_of_length = Tools::nextIndex(t, i + 2, ",)");
                    if (t[i + 1] == ':') {
                        length = Tools::substring(t, i + 2, end_of_length);
                    }
                    else {
                        length = LENGTH_DEFAULT;
                    }
                    q.front().setAttribute(EdgeAttribute(length));
                    edges.push_back(q.front());
                    q.pop_front();
                    i = end_of_length;
                    break;
                }

                case ',': {
                    i++;
                    break;
                }

                    // this char is the beginning of a leaf name
                default: {
                    end_of_label = Tools::nextIndex(t, i, ":");

                    // check if 'correct' :-delimited length is missing
                    alt_end_of_label = Tools::nextIndex(t, i, " ,()");
                    if (alt_end_of_label < end_of_label) {
                        end_of_label = end_of_length = alt_end_of_label;
                        length = LENGTH_DEFAULT;
                    }
                    else {
                        end_of_length = Tools::nextIndex(t, end_of_label, " ,)");
                        length = Tools::substring(t, end_of_label + 1, end_of_length);
                    }
                    label = Tools::substring(t, i, end_of_label);
                    leafNum = lower_bound(leaf2NumMap.begin(), leaf2NumMap.end(), label) - leaf2NumMap.begin();
                    if (leafNum == leaf2NumMap.size()) throw out_of_range("Could not find label (" + label + ") in leaf2NumMap");
                    leafEdgeAttribs[leafNum] = EdgeAttribute(length);

                    for (auto &e: q) {
                        e.addOne((size_t) leafNum);
                    }
                    i = end_of_length;
                }
            }
        }
    }
    catch (out_of_range &range_err) {
        throw range_err;
    }

    if (!rooted) {
        for (auto &e : edges) {
            size_t lastLeaf = leaf2NumMap.size() - 1;
            if (e.contains(lastLeaf)) {
                e.complement(lastLeaf + 1);
            }
        }
    }

    for (size_t k = 0; k < edges.size(); ++k) {
        edges[k].setOriginalEdge(edges[k].asSplit());
        edges[k].setOriginalID(k);
    }
}

vector<PhyloTreeEdge> PhyloTree::getCommonEdges(PhyloTree &t1, PhyloTree &t2) {
    vector<PhyloTreeEdge> commonEdges;
    EdgeAttribute l_attr, r_attr;
    if (t1.leaf2NumMap != t2.leaf2NumMap) {
        throw runtime_error("leaf2NumMaps are not equal");
    }
    for (auto e1 : t1.edges) {
        if (!e1.isZero()) {
            auto splits = t2.getSplits();
            if (std::find(splits.begin(), splits.end(), e1.asSplit()) != splits.end()) {
                l_attr = e1.getAttribute();
                r_attr = t2.getAttribOfSplit(e1.asSplit());
                auto commonAttrib = EdgeAttribute::difference(l_attr, r_attr);
                commonEdges.push_back(PhyloTreeEdge(e1.asSplit(), commonAttrib, e1.getOriginalID()));
            }
            else if (e1.isCompatibleWith(t2.getSplits())) {
                auto commonAttrib = e1.getAttribute();
                commonEdges.push_back(PhyloTreeEdge(e1.asSplit(), commonAttrib, e1.getOriginalID()));
            }
        }
    }
    for (auto e2 : t2.getEdges()) {
        if (!e2.isZero()) {
            auto splits = t1.getSplits();
            if (e2.isCompatibleWith(t1.getSplits()) && std::find(splits.begin(), splits.end(), e2.asSplit()) == splits.end()) {
                auto commonAttrib = e2.getAttribute();
                commonEdges.push_back(PhyloTreeEdge(e2.asSplit(), commonAttrib, e2.getOriginalID()));
            }
        }
    }
    return commonEdges;
}

vector<PhyloTreeEdge> PhyloTree::getEdges() {
    return edges;
}

void PhyloTree::setEdges(vector<PhyloTreeEdge> edges) {
    this->edges = edges;
}

PhyloTreeEdge PhyloTree::getEdge(size_t i) {
    return edges[i];
}

vector<string> PhyloTree::getLeaf2NumMap() {
    return leaf2NumMap;
}

void PhyloTree::setLeaf2NumMap(vector<string> leaf2NumMap) {
    this->leaf2NumMap = leaf2NumMap;
}

EdgeAttribute PhyloTree::getAttribOfSplit(Bipartition edge) {
    for (auto e : edges) {
        if (e.sameBipartition(edge)) {
            return e.getAttribute();
        }
    }
    return EdgeAttribute();
}

vector<Bipartition> PhyloTree::getSplits() {
    vector<Bipartition> splits;

    for (auto &edge : edges) {
        splits.push_back(edge.asSplit());
    }

    return splits;
}

void PhyloTree::normalize() {
    double vecLength = getDistanceFromOrigin();
    normalize(vecLength);
}

void PhyloTree::normalize(PhyloTree &other) {
    double combinedVecLength = getDistanceFromOrigin() + other.getDistanceFromOrigin();
    normalize(combinedVecLength);
    other.normalize(combinedVecLength);
}

size_t PhyloTree::numLeaves() {
    return leaf2NumMap.size();
}

double PhyloTree::getDistanceFromOrigin() {
    double dist = 0;
    for (auto &edge : edges) {
        dist += std::pow(edge.getLength(), 2);
    }

    for (auto &attrib : leafEdgeAttribs) {
        dist += std::pow(attrib.norm(), 2);
    }

    return std::sqrt(dist);
}

double PhyloTree::getDistanceFromOriginNoLeaves() {
    double dist = 0;
    for (auto &edge : edges) {
        dist += std::pow(edge.getLength(), 2);
    }
    return std::sqrt(dist);
}

double PhyloTree::getBranchLengthSum() {
    double sum = 0;
    for (auto &edge : edges) {
        sum += edge.getLength();
    }

    for (auto &attrib : leafEdgeAttribs) {
        sum += attrib.norm();
    }

    return sum;
}

vector<PhyloTreeEdge> PhyloTree::getEdgesNotInCommonWith(PhyloTree t) {
    vector<PhyloTreeEdge> notCommonEdges;
    bool not_common;
    if (leaf2NumMap != t.leaf2NumMap) {
        throw runtime_error("leaf2NumMaps are not equal");
    }
    for (auto this_edge : edges) {
        not_common = true;
        if (this_edge.isZero()) continue;

        for (auto &that_edge : t.edges) {
            if (that_edge.isZero()) continue;
            if (this_edge.sameBipartition(that_edge)) {
                not_common = false;
            }
        }
        if (not_common)
            notCommonEdges.push_back(this_edge);
    }
    return notCommonEdges;
}

//vector<Bipartition> PhyloTree::getCrossingsWith(PhyloTree t) {
//    return std::vector<Bipartition, allocator<Bipartition>>();
//}

//string PhyloTree::toString() {
//    return string();
//}

PhyloTree PhyloTree::clone() {
    return PhyloTree(*this);
}

bool PhyloTree::equals(PhyloTree t) {
    return (leaf2NumMap == t.leaf2NumMap) && Tools::vector_equal(leafEdgeAttribs, t.leafEdgeAttribs) && Tools::vector_equal(edges, t.edges);
}

//bool PhyloTree::approxEquals(PhyloTree t, double epsilon) {
//    return false;
//}

vector<EdgeAttribute> PhyloTree::getLeafEdgeAttribs() {
    return leafEdgeAttribs;
}

void PhyloTree::setLeafEdgeAttribs(vector<EdgeAttribute> otherEdgeAttribs) {
    leafEdgeAttribs.clear();
    for (auto e : otherEdgeAttribs) {
        leafEdgeAttribs.push_back(e);
    }
}

vector<EdgeAttribute> PhyloTree::getCopyLeafEdgeAttribs() {
    return std::vector<EdgeAttribute, allocator<EdgeAttribute>>();
}

vector<double> PhyloTree::getIntEdgeAttribNorms() {
    vector<double> norms;
    for (auto &e : edges) {
        norms.push_back(e.getAttribute().norm());
    }
    return norms;
}

string PhyloTree::getNewick(bool branchLengths) {
    deque<string> strPieces;
    deque<PhyloTreeEdge> corrEdges;

    if (edges.size() == 0) // star-tree
    {
        strPieces.push_back("(");
        for (size_t i = 0; i < leaf2NumMap.size() - 1; ++i) {
            strPieces.push_back(leaf2NumMap[i]);
            if (branchLengths) {
                strPieces.push_back(":");
                strPieces.push_back(Tools::double_to_string(leafEdgeAttribs[i].getAttribute()));
            }
            strPieces.push_back(",");
        }
        strPieces.push_back(leaf2NumMap.back());
        strPieces.push_back(":");
        strPieces.push_back(Tools::double_to_string(leafEdgeAttribs.back().getAttribute()));
        strPieces.push_back(");");
        return Tools::string_join(strPieces, "");
    }

    vector<PhyloTreeEdge> edgesLeft(edges);

    while (edgesLeft.size() > 0) {
        auto minEdge = edgesLeft.front();
        size_t pos = 0;
        for (size_t i = 1; i < edgesLeft.size(); ++i) {
            if (minEdge.contains(edgesLeft[i])) {
                minEdge = edgesLeft[i];
                pos = i;
            }
        }
        Tools::vector_remove_element_at_index(edgesLeft, pos);

        corrEdges.push_front(minEdge.clone());
        strPieces.push_front("");

        // now we have a min element.
        // Start its Newick string.
        string str1 = "(";

        // Since we are allowing degenerate trees, there could be an arbitrary number of such edges.
        size_t k = 1;
        while (k < corrEdges.size()) {
            if (minEdge.contains(corrEdges[k])) {
                // add it to the string
                str1 += strPieces[k] + ",";

                // remove each leaf in this split from minEdge
                minEdge.andNot(corrEdges[k]);

                // remove this split and its corresponding string from the vectors
                Tools::deque_remove_element_at_index(strPieces, k);
                Tools::deque_remove_element_at_index(corrEdges, k);
            } else {
                k++;
            }
        }

        // add all the elements still in minEdge (These are leaves that weren't already added as part of
        // a min split contained by minEdge.)
        if (!minEdge.getPartition()->empty()) {
            for (size_t i = 0; i < minEdge.getPartition()->size(); i++) {
                if ((*(minEdge.getPartition()))[i]) {
                    str1 += leaf2NumMap[i];
                    if (branchLengths) {
                        str1 += ":" + Tools::double_to_string(leafEdgeAttribs[i].getAttribute());
                    }
                    str1 += ",";
                }
            }
        }
        // remove the last , and add the bracket and minEdge length
        str1 = Tools::substring(str1, 0, str1.length() - 1) + ")";
        if (branchLengths) {
            str1 += ":" + Tools::double_to_string(minEdge.getAttribute().getAttribute());
        }
        // store str1
        strPieces.push_front(str1);
    }

    // now we need to combine all edges in corrEdges and all remaining leaves
    Bipartition allLeaves = Bipartition(string(leaf2NumMap.size(), '1'));

    string newickString = "(";
    // add all the string pieces we've accumulated
    for (int i = 0; i < corrEdges.size(); i++) {
        newickString += strPieces[i] + ",";
        allLeaves.andNot(corrEdges[i]);
    }
    // add all remaining leaves
    if (!allLeaves.isEmpty()) {
        for (size_t i = 0; i < allLeaves.size(); i++) {
            if ((*(allLeaves.getPartition()))[i]) {
                newickString = newickString + leaf2NumMap[i];
                if (branchLengths) {
                    newickString += ":" + Tools::double_to_string(leafEdgeAttribs[i].getAttribute());
                }
                newickString += ",";
            }
        }
    }

    // remove the last ,
    newickString = Tools::substring(newickString, 0, newickString.length() - 1) + ");";
    return newickString;
}

void PhyloTree::setNewick(string newick) {
    this->newick = newick;
}

int PhyloTree::numEdges() {
    return edges.size();
}

void PhyloTree::setLeaf2NumMapFromNewick() {
    // go through the string and pull out all the leaf labels:  any string between '(' and ':' or ',' and ':'
    size_t i = 0;
    vector<string> split_newick = Tools::string_split(newick, " \t\n", "");
    string despaced_newick = Tools::string_join(split_newick, "");
    while (i < despaced_newick.length()) {
        // however, the first character might be the beginning of a leaf label
        if ((despaced_newick[i] == '(' || despaced_newick[i] == ',') && (despaced_newick[i + 1] != '(')) {
            size_t end_of_label = Tools::nextIndex(despaced_newick, i, ":");
            string label = Tools::substring(despaced_newick, i + 1, end_of_label);
            leaf2NumMap.push_back(label);
            i = Tools::nextIndex(despaced_newick, i, ",)");
        } else {
            i++;
        }
    }
    // sort the elements of leaf2NumMap
    std::sort(leaf2NumMap.begin(), leaf2NumMap.end());
}

void PhyloTree::normalize(double constant) {
    // divide by the length of the split length vector to normalize
    for (int i = 0; i < leafEdgeAttribs.size(); i++) {
        leafEdgeAttribs[i].scaleBy(1.0 / constant);
    }
    for (int i = 0; i < edges.size(); i++) {
        edges[i].getAttribute().scaleBy(1.0 / constant);
    }
}

bool PhyloTree::removeSplit(Bipartition e) {
    bool removed = false;

    for (size_t i = 0; i < edges.size(); i++) {
        if (edges[i].sameBipartition(e)) {
            Tools::vector_remove_element_at_index(edges, i);
            removed = true;
            break;
        }
    }
    return removed;
}

void PhyloTree::removeSplits(vector<Bipartition> splits) {
    for (auto &split : splits) {
        removeSplit(split);
    }
}

string PhyloTree::toString() {
    size_t nlabels = leaf2NumMap.size();
    size_t nedges = edges.size();
    size_t nleaves = leafEdgeAttribs.size();
    if (nlabels == nedges == nleaves == 0) return "";
    stringstream ss;
    ss << "Leaves: ";
    for (size_t i = 0; i < nlabels - 1; ++i) {
        ss << leaf2NumMap[i] << " ";
    }
    ss << leaf2NumMap.back();
    if (nedges > 0) {
        ss << "; edges: ";
        for (size_t i = 0; i < edges.size() - 1; ++i) {
            ss << edges[i].toString() << " ";
        }
        ss << edges.back().toString();
    }
    if (nleaves > 0) {
        ss << "; leaf edges: [";
        for (size_t i = 0; i < nleaves - 1; ++i) {
            ss << leafEdgeAttribs[i].toString() << " ";
        }
        ss << leafEdgeAttribs.back().toString() << "]";
    }
    return ss.str();
}

double PhyloTree::getDistanceFromOrigin() const {
    double dist = 0;
    for (auto &edge : edges) {
        dist += std::pow(edge.getLength(), 2);
    }

    for (auto &attrib : leafEdgeAttribs) {
        dist += std::pow(attrib.norm(), 2);
    }

    return std::sqrt(dist);
}
