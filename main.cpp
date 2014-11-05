#include "Distance.h"

int main(int argc, char const *argv[]) {
    string s1("((((a:1,b:1):1,c:1):1,d:1):1,(i:1,(j:1,k:1):1):1,(l:1,(h:1,(g:1,(e:1,f:1):1):1):1):1);");
    string s2("((((a:2,b:2):2,c:2):2,d:2):2,(h:2,(g:2,(e:2,f:2):2):2):2,(i:2,(j:2,(k:2,l:2):2):2):2);");
    string s3("(g:1,(a:1,(b:1,c:1):1):1,(f:1,(e:1,d:1):1):1);");
    string s4("(g:2,(a:2,(b:2,c:2):2):2,(d:2,(e:2,f:2):2):2);");
    string s5("(((((Sp1:0.237938301594,Sp2:0.237938301594):0.652150465533,Sp3:0.890088767127):1.0032766685,(Sp4:1.19159193346,(((Sp5:0.0499647780388,Sp6:0.0499647780388):0.625094094731,(Sp7:0.385992882857,(Sp8:0.0229771776543,Sp9:0.0229771776543):0.363015705203):0.289065989913):0.100330232606,Sp10:0.775389105376):0.416202828082):0.70177350217):1.37956852633,(((Sp11:0.143770218098,Sp12:0.143770218098):2.22399819333,(Sp13:0.879389128685,Sp14:0.879389128685):1.48837928275):0.882615734883,((((((Sp15:0.115537491571,Sp16:0.115537491571):1.14083013878,(Sp17:0.787001759928,((Sp18:0.109471903715,Sp19:0.109471903715):0.647746750411,Sp20:0.757218654126):0.029783105802):0.469365870425):0.210543598558,((Sp21:0.0928829593343,Sp22:0.0928829593343):0.596056952564,(Sp23:0.295394029748,Sp24:0.295394029748):0.39354588215):0.777971317013):0.273542049999,(((Sp25:0.750938536668,((Sp26:0.147146981319,Sp27:0.147146981319):0.197746581084,Sp28:0.344893562404):0.406044974265):0.224512201078,Sp29:0.975450737746):0.568823245523,(Sp30:0.423829830969,Sp31:0.423829830969):1.1204441523):0.196179295641):0.204967720146,(Sp32:1.88686786295,Sp33:1.88686786295):0.0585531361112):0.782778416912,((Sp34:0.453020826526,Sp35:0.453020826526):1.00864842727,Sp36:1.4616692538):1.26653016217):0.522184730348):0.0225498156455):0.118838892224,((Sp37:1.61330136153,((Sp38:1.11318444903,Sp39:1.11318444903):0.488085401285,(Sp40:1.42576644618,(((Sp41:0.0435502147095,Sp42:0.0435502147095):0.233013603141,Sp43:0.276563817851):0.175951351205,(Sp44:0.0424687878211,Sp45:0.0424687878211):0.410046381235):0.973251277121):0.175503404136):0.0120315112168):1.10970378837,((((Sp46:0.0108728387409,Sp47:0.0108728387409):0.096790292957,Sp48:0.107663131698):0.575955539077,Sp49:0.683618670775):0.35904620643,Sp50:1.0426648772):1.68034027269):0.66876770429):0.0;");
    string s6("(((((Sp1:0.237938301594,Sp2:0.237938301594):0.652150465533,(Sp4:1.19159193346,(Sp10:0.775389105376,(Sp7:0.385992882857,((Sp5:0.0499647780388,Sp6:0.0499647780388):0.625094094731,(Sp8:0.0229771776543,Sp9:0.0229771776543):0.363015705203):0.289065989913):0.100330232606):0.416202828082):0.70177350217):1.0032766685,Sp3:0.890088767127):1.37956852633,(((Sp11:0.143770218098,Sp12:0.143770218098):2.22399819333,(Sp13:0.879389128685,Sp14:0.879389128685):1.48837928275):0.882615734883,(((Sp34:0.453020826526,Sp35:0.453020826526):1.00864842727,Sp36:1.4616692538):1.26653016217,((Sp32:1.88686786295,Sp33:1.88686786295):0.0585531361112,((((Sp21:0.0928829593343,Sp22:0.0928829593343):0.596056952564,(Sp23:0.295394029748,Sp24:0.295394029748):0.39354588215):0.777971317013,((Sp15:0.115537491571,Sp16:0.115537491571):1.14083013878,((Sp20:0.757218654126,Sp17:0.787001759928):0.029783105802,(Sp18:0.109471903715,Sp19:0.109471903715):0.647746750411):0.469365870425):0.210543598558):0.273542049999,((Sp30:0.423829830969,Sp31:0.423829830969):1.1204441523,(Sp29:0.975450737746,((Sp26:0.147146981319,Sp27:0.147146981319):0.197746581084,(Sp25:0.750938536668,Sp28:0.344893562404):0.406044974265):0.224512201078):0.568823245523):0.196179295641):0.204967720146):0.782778416912):0.522184730348):0.0225498156455):0.118838892224,((Sp37:1.61330136153,((Sp38:1.11318444903,Sp39:1.11318444903):0.488085401285,(Sp40:1.42576644618,(((Sp41:0.0435502147095,Sp42:0.0435502147095):0.233013603141,Sp43:0.276563817851):0.175951351205,(Sp44:0.0424687878211,Sp45:0.0424687878211):0.410046381235):0.973251277121):0.175503404136):0.0120315112168):1.10970378837,((((Sp46:0.0108728387409,Sp47:0.0108728387409):0.096790292957,Sp48:0.107663131698):0.575955539077,Sp49:0.683618670775):0.35904620643,Sp50:1.0426648772):1.68034027269):0.66876770429):0.0;");
    auto t1 = PhyloTree(s1, false);
    auto t2 = PhyloTree(s2, false);
    auto t3 = PhyloTree(s3, false);
    auto t4 = PhyloTree(s4, false);
    auto t5 = PhyloTree(s5, true);
    auto t6 = PhyloTree(s6, true);

    clock_t start = clock();
    for (size_t i = 0; i < 100; ++i) {
        Distance::getEuclideanDistance(t5, t6, false);
        Distance::getGeodesicDistance(t5, t6, false);
        Distance::getRobinsonFouldsDistance(t5, t6, false);
        Distance::getWeightedRobinsonFouldsDistance(t5, t6, false);
    }
    printf("Time taken: %.3f millisec\n", 1000 * (double) (clock() - start) / CLOCKS_PER_SEC);
    return 0;
}
