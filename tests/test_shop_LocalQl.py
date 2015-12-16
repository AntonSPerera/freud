from freud import locality, trajectory
from freud import sphericalharmonicorderparameters as shop
import numpy as np
import numpy.testing as npt
import unittest

def FCC256():
    fcc256 = np.array([[-5.41529235,-4.91528249,-6.39377763],
[-3.46538746,-4.95965734,-8.74656397],
[-3.18963384,-6.93118316,-6.51684528],
[-5.63089642,-7.27349965,-8.65186427],
[-1.17064858,-7.04437845,-8.72344406],
[-5.51052462,-2.73711317,-8.70209229],
[-1.18050931,-2.63083916,-8.52551427],
[-5.40588822,-7.27490581,-4.40783645],
[-1.23052923,-7.01813524,-4.27817809],
[-5.42753562,-2.75372452,-4.31410833],
[-1.12811679,-2.61692349,-4.10876553],
[-7.68841724,-7.27322055,-6.47162298],
[-3.29901376,-2.73588866,-6.48736601],
[-7.609418,-2.85268546,-6.41298006],
[-3.33116835,-7.26034364,-2.16427247],
[-7.77182688,-7.15761628,-2.05557521],
[-3.26132427,-2.79856172,-2.0318182],
[-7.68993287,-2.59680603,-2.13837143],
[-7.5688227,-4.93485848,-8.6991062],
[-3.36343037,-0.50631226,-8.50467493],
[-7.89276058,-0.41458778,-8.81914403],
[-3.2998955,-4.82605939,-4.1664717],
[-7.67950846,-5.11083673,-4.26627114],
[-3.36331565,-0.54838112,-4.29748461],
[-7.63653303,-0.47706391,-4.41039437],
[-1.13747351,-4.76156074,-6.39715157],
[-5.57680217,-0.71638777,-6.43815819],
[-1.03608577,-0.36452625,-6.64054963],
[-5.65961491,-4.87821351,-2.01985673],
[-1.13434115,-4.87889349,-2.03955817],
[-5.50268461,-0.58674995,-2.08366193],
[-1.17860986,-0.52100751,-2.07676245],
[7.72274732,-0.4326282,-2.35240038],
[-1.06199633,8.45666224,-2.13245235],
[7.85683756,8.27678829,-2.22552919],
[-1.21432945,-0.518902,6.67771583],
[7.71879302,-0.32592052,6.50427644],
[-1.13982376,8.30837496,6.69182179],
[7.88870886,8.43309006,6.7458642],
[3.26030168,-0.55543782,-1.8621176],
[-5.30002951,8.27182761,-2.15217606],
[3.37733667,8.44801485,-2.12279967],
[-5.43793675,-0.5743545,6.79634073],
[3.35096956,-0.63471875,6.719801],
[-5.48126439,8.53348003,6.73212035],
[3.32076267,8.32443525,6.8559488],
[7.84345039,-4.99024033,-2.17884111],
[-1.06462136,3.93224212,-2.14189443],
[7.71549683,3.80551302,-1.88955802],
[-1.33191662,-4.9127534,6.87465309],
[7.86797733,-4.99733951,6.83474402],
[-1.11576783,3.92672741,6.78453828],
[7.79503623,3.94509239,6.92854783],
[3.24044613,-4.98452893,-2.1853755],
[-5.51006088,3.70742013,-2.00144131],
[3.21250619,3.95883959,-2.11135998],
[-5.62399457,-4.71981073,6.61342546],
[3.18498574,-4.91534036,6.62663439],
[-5.45458174,3.75714441,6.73061412],
[3.27096153,3.95509896,6.87827725],
[7.58438649,-0.49817053,-6.66289477],
[-1.11913663,8.39011979,-6.64050321],
[7.77734228,8.25606361,-6.34753085],
[-1.34459689,-0.33590773,2.36352131],
[7.70368362,-0.59268102,2.34335157],
[-1.06731568,8.30536563,2.31059156],
[7.71318488,8.29286034,2.29476324],
[3.41241203,-0.44532785,-6.50041194],
[-5.53301505,8.21187384,-6.51282981],
[3.31235904,8.35438577,-6.61196348],
[-5.63259494,-0.39971095,2.31831889],
[3.26645513,-0.40245877,2.29940551],
[-5.52919525,8.38114836,2.30268976],
[3.34763488,8.39562797,2.19937459],
[7.51228977,-4.93919217,-6.50106691],
[-1.03993818,4.04462196,-6.31182572],
[7.84400721,4.01971665,-6.57485312],
[-1.04391646,-5.04263697,2.4858331],
[7.60820361,-4.87066578,2.27299704],
[-0.99816369,4.02944264,2.30744125],
[7.5793462,4.07016307,2.4815974],
[0.896220018,-0.38390263,-4.18196839],
[-7.69000041,8.47831893,-4.22047149],
[1.16549219,8.36790573,-4.20470295],
[-7.79326119,-0.39922396,4.50754392],
[0.944380627,-0.55337008,4.663775],
[-7.64736762,8.37377732,4.55325925],
[1.05197804,8.14319578,4.43806427],
[5.37099797,-0.34173551,-4.26643814],
[-3.09220939,8.21880844,-4.40065938],
[5.39503608,8.23159824,-4.41439508],
[-3.41545474,-0.61069315,4.60951747],
[5.50102363,-0.55662444,4.57626201],
[-3.41699511,8.29748391,4.5687663],
[5.5027255,8.3411649,4.50260423],
[0.937123884,-4.74140558,-4.21864507],
[-7.72641201,3.77641955,-4.35812245],
[1.21147044,3.91923311,-4.25055873],
[-7.7905882,-4.92832813,4.59909129],
[1.14062864,-4.92829816,4.48888404],
[-7.66610956,3.88346107,4.53307107],
[1.13473391,4.06950732,4.59328606],
[5.46931768,-4.74224426,-4.34549057],
[-3.28508976,3.81370926,-4.2613304],
[5.45583389,3.8965082,-4.29905737],
[-3.39185004,-5.05737712,4.59038641],
[5.32979581,-4.90622931,4.4910936],
[-3.3541199,4.11076237,4.64146239],
[5.54891161,4.00732014,4.54804957],
[1.05404395,-0.41871244,8.7295758],
[-7.65516676,8.21430331,-8.59424258],
[1.05538191,8.44693396,-8.73080759],
[-7.73566522,-0.69306911,0.103093841],
[0.954361853,-0.5034737,0.048733928],
[-7.73611356,8.27929524,0.230705593],
[1.07687363,8.34860511,0.089977987],
[5.46940726,-0.68669955,-8.79181942],
[-3.33873472,8.61713365,-8.67060054],
[5.53256864,8.53174826,-8.60671725],
[-3.45518481,-0.53184318,0.164418812],
[5.68526718,-0.6342028,0.086672057],
[-3.19566801,8.30450622,-0.01810068],
[5.64381275,8.24580494,0.105887639],
[1.02745244,-4.7918141,-8.77631639],
[-7.60660449,4.12109404,8.82737661],
[1.10870395,3.76425207,-8.53537707],
[-7.8390076,-5.04553376,0.035622241],
[1.09045849,-5.00752535,0.206768214],
[-7.72651968,3.86151361,0.316460597],
[1.05867865,3.88742584,0.092820668],
[1.07216502,-2.61677045,-2.1347661],
[-7.64834723,5.90895067,-2.13045622],
[1.07837175,6.16200557,-1.90698119],
[-7.82745871,-2.73023598,6.84240945],
[0.853193254,-2.82826516,6.67202807],
[-7.57588895,6.33352181,6.65593506],
[1.09982269,6.15451902,6.7708181],
[5.74680705,-2.73279315,-2.07784657],
[-3.18135495,6.04316172,-2.17299009],
[5.68073028,6.15673268,-2.2540577],
[-3.37323723,-2.76548291,6.83434842],
[5.51943443,-2.65161524,6.66248482],
[-3.3923294,6.19414453,6.75660554],
[5.54803566,6.14575781,6.70551919],
[1.08256163,-7.09407614,-2.1844661],
[-7.62660249,1.59218088,-1.98925646],
[1.0747759,1.77658748,-2.03096471],
[-7.70876622,-7.17411968,6.70463094],
[1.0590199,-7.0761226,6.68037448],
[-7.72997884,1.92271431,6.85604915],
[0.98874676,1.67578929,6.69315216],
[5.59858006,-7.19353252,-2.34651593],
[-3.24993646,1.63021488,-1.82190252],
[5.61857058,1.67026284,-1.98346462],
[-3.38111713,-7.04482023,6.83972967],
[5.54381406,-7.08037461,6.66546906],
[-3.33084599,1.64229757,6.93921312],
[5.45977961,1.58026681,6.84856281],
[1.21506314,-2.39621933,-6.53329636],
[-7.64022498,6.11631666,-6.40393223],
[1.11014125,6.26851723,-6.54105517],
[-7.8489321,-2.86267076,2.36822799],
[0.918376479,-2.5229399,2.46927762],
[-7.82694669,5.99790381,2.44183173],
[1.12681313,6.11789887,2.30228219],
[5.65787708,-2.67475166,-6.56536675],
[-3.12375629,6.08488593,-6.51516538],
[5.44269889,6.01450094,-6.56165174],
[-3.3523847,-2.54588909,2.36083487],
[5.39264312,-2.75614773,2.449411],
[-3.25686096,6.26232471,2.27649206],
[5.23147883,6.23288201,2.24545902],
[0.918448668,-7.13460467,-6.56768916],
[-7.66840086,1.59477164,-6.51310802],
[1.11656355,1.77217963,-6.43293331],
[-7.79627355,-7.17682097,2.16482166],
[1.12530115,-7.3351826,2.38731026],
[-7.83105322,1.56775873,2.34066019],
[1.27489912,1.81191103,2.41309776],
[7.67615689,-2.6386199,-4.32584674],
[-0.72709532,6.1111759,-4.24466418],
[7.80971105,6.09827882,-4.28457428],
[-1.3635662,-2.70193845,4.6277647],
[7.79511811,-2.73621334,4.53645432],
[-1.09750098,6.12138858,4.30007131],
[7.75017136,6.06107382,4.63967329],
[3.13909918,-2.80684093,-4.25921698],
[-5.35568756,6.03351177,-4.39422034],
[3.26170379,6.14785893,-4.35625714],
[-5.47659497,-2.75754824,4.47971471],
[3.21586274,-2.7430501,4.5702104],
[-5.48810058,6.09274892,4.37520838],
[3.18170864,6.25013972,4.58798339],
[7.85518819,-7.14003985,-4.23617272],
[-1.10344809,1.71382148,-4.43184477],
[7.63139394,1.74105295,-4.28408735],
[-1.04577689,-7.16451017,4.54532251],
[7.59012206,-7.00961706,4.41720935],
[-1.01033104,1.91011018,4.4754523],
[7.77065004,2.02282119,4.53756167],
[3.32033033,-7.23712048,-4.25979223],
[-5.44956492,1.74683176,-4.09399723],
[3.16613295,1.59406072,-4.18272001],
[-5.71871464,-7.11595484,4.49872517],
[3.15367497,-7.30955209,4.53772536],
[-5.61565131,1.68414663,4.51152458],
[3.29511374,1.67157897,4.82523097],
[7.79320933,-2.72394909,-8.67613559],
[-0.91272871,6.17346116,-8.72390357],
[7.83806566,6.18136234,-8.76278593],
[-1.1308233,-2.63023604,0.111838049],
[7.8486327,-2.74775406,0.063802672],
[-1.09217344,6.21090952,0.312339399],
[7.86112543,6.029112,0.282238084],
[3.3913359,-2.84329474,-8.74090455],
[-5.45729796,6.09999621,-8.64953209],
[3.37696394,6.16755485,-8.73165003],
[-5.56177703,-2.71847034,0.21872864],
[3.28463781,-2.86673822,0.045186999],
[-5.52723232,6.20641977,-0.03307247],
[3.1206309,6.10579388,0.209776673],
[7.72412755,-7.11444685,-8.54059454],
[-1.25930372,1.72985786,-8.67411378],
[7.72744636,1.76513022,-8.55935639],
[-0.96418304,-7.0630432,0.043531002],
[7.56006278,-7.06387943,0.140478649],
[-0.92419354,1.73862506,0.296837641],
[7.87534628,1.52180225,0.155017513],
[3.23880954,-6.95200943,-8.78963123],
[-5.42198428,1.8131953,-8.64036039],
[3.16031242,1.65305305,-8.66046381],
[-5.48682421,-7.13325103,0.016941701],
[3.20810495,-7.0021079,0.04967968],
[-5.66516326,1.72331098,0.343829399],
[3.17686646,1.76800772,0.085435207],
[5.34698383,-7.10039518,-6.39997894],
[-3.37741642,1.76552396,-6.4467159],
[5.39731292,1.82916332,-6.55471599],
[-3.39222916,-7.1758926,2.47669247],
[5.40515314,-7.05923991,2.35360007],
[-3.41689039,1.75564058,2.2815387],
[5.6764043,1.7314,2.25472487],
[5.63529441,-4.86878005,8.74783235],
[-3.36351743,4.01356773,-8.57900101],
[5.49841747,3.9485758,-8.64459608],
[-3.22764721,-5.01448777,0.151530548],
[5.37303167,-4.99907199,0.228485783],
[-3.19264181,4.11609892,0.187395327],
[5.55785115,3.87474569,0.139998049],
[3.23512013,-5.00979661,-6.37749004],
[-5.57509596,3.79062104,-6.34718999],
[3.21608207,3.90019806,-6.48732855],
[-5.7491254,-4.91459834,2.20083258],
[3.3168185,-4.88054616,2.39202468],
[-5.42558238,3.97608203,2.30259428],
[3.28158615,3.99298902,2.41987272]],dtype=np.float32);
    return fcc256;

class TestLocalQl(unittest.TestCase):
    def test_Qlfcc(self):
        rcut = 3.7;
        testpoints = FCC256();
        box = trajectory.Box(17.661,17.661,17.661);

        localq4 = shop.LocalQl(box, rcut, 4);
        localq4.compute(testpoints);
        q4vals = localq4.getQl();
        meanq4 = np.mean(q4vals);

        localq6 = shop.LocalQl(box, rcut, 6);
        localq6.compute(testpoints);
        q6vals = localq6.getQl();
        meanq6 = np.mean(q6vals);

        localq8 = shop.LocalQl(box, rcut, 8);
        localq8.compute(testpoints);
        q8vals = localq8.getQl();
        meanq8 = np.mean(q8vals);

        localq10 = shop.LocalQl(box, rcut, 10);
        localq10.compute(testpoints);
        q10vals = localq10.getQl();
        meanq10 = np.mean(q10vals);

        #Assert these tests have same value as literature
        #In Steinhardt 1983 (DOI: 10.1103/PhysRevB.28.784)  Fig2 these q are ~0.2, 0.56, 0.4, (hard to read -tiny).
        #  Note:  Not given exactly as a table, but agreement is within a few percent for l=4,6,8,10.
        npt.assert_almost_equal(meanq4, 0.19, decimal=2, err_msg="Q4fail")
        npt.assert_almost_equal(meanq6, 0.56, decimal=2, err_msg="Q6fail")
        npt.assert_almost_equal(meanq8, 0.38, decimal=2, err_msg="Q8fail")
        npt.assert_almost_equal(meanq10, 0.09, decimal=2, err_msg="Q10fail")

class TestLocalAveQl(unittest.TestCase):
    def test_AveQlfcc(self):
        rcut = 3.7;
        testpoints = FCC256();
        box = trajectory.Box(17.661,17.661,17.661);

        localq4 = shop.LocalQl(box, rcut, 4);
        localq4.computeAve(testpoints);
        q4vals = localq4.getAveQl();
        meanq4 = np.mean(q4vals);

        localq6 = shop.LocalQl(box, rcut, 6);
        localq6.computeAve(testpoints);
        q6vals = localq6.getAveQl();
        meanq6 = np.mean(q6vals);

        localq8 = shop.LocalQl(box, rcut, 8);
        localq8.computeAve(testpoints);
        q8vals = localq8.getAveQl();
        meanq8 = np.mean(q8vals);

        localq10 = shop.LocalQl(box, rcut, 10);
        localq10.computeAve(testpoints);
        q10vals = localq10.getAveQl();
        meanq10 = np.mean(q10vals);

        #Assert these tests have same value as literature
        #In Steinhardt 1983 (DOI: 10.1103/PhysRevB.28.784)  Fig2 these q are ~0.2, 0.56, 0.4, (hard to read -tiny).
        #  Note:  Not given exactly as a table, but agreement is within a few percent for l=4,6,8,10.
        npt.assert_almost_equal(meanq4, 0.19, decimal=2, err_msg="AveQ4fail")
        npt.assert_almost_equal(meanq6, 0.55, decimal=2, err_msg="AveQ6fail")
        npt.assert_almost_equal(meanq8, 0.38, decimal=2, err_msg="AveQ8fail")
        npt.assert_almost_equal(meanq10, 0.02, decimal=2, err_msg="AveQ10fail")

class TestLocalQlNorm(unittest.TestCase):
    def test_QlNormfcc(self):
        rcut = 3.7;
        testpoints = FCC256();
        box = trajectory.Box(17.661,17.661,17.661);

        localq4 = shop.LocalQl(box, rcut, 4);
        localq4.computeNorm(testpoints);
        q4vals = localq4.getQlNorm();
        meanq4 = np.mean(q4vals);

        localq6 = shop.LocalQl(box, rcut, 6);
        localq6.computeNorm(testpoints);
        q6vals = localq6.getQlNorm();
        meanq6 = np.mean(q6vals);

        localq8 = shop.LocalQl(box, rcut, 8);
        localq8.computeNorm(testpoints);
        q8vals = localq8.getQlNorm();
        meanq8 = np.mean(q8vals);

        localq10 = shop.LocalQl(box, rcut, 10);
        localq10.computeNorm(testpoints);
        q10vals = localq10.getQlNorm();
        meanq10 = np.mean(q10vals);

        #Assert these tests have same value as literature
        #In Steinhardt 1983 (DOI: 10.1103/PhysRevB.28.784)  Fig2 these q are ~0.2, 0.56, 0.4, (hard to read -tiny).
        #  Note:  Not given exactly as a table, but agreement is within a few percent for l=4,6,8,10.
        npt.assert_almost_equal(meanq4, 0.19, decimal=2, err_msg="Q4Normfail")
        npt.assert_almost_equal(meanq6, 0.55, decimal=2, err_msg="Q6Normfail")
        npt.assert_almost_equal(meanq8, 0.38, decimal=2, err_msg="Q8Normfail")
        npt.assert_almost_equal(meanq10, 0.01, decimal=2, err_msg="Q10Normfail")

class TestLocalAveNormQl(unittest.TestCase):
    def test_AveNormQlfcc(self):
        rcut = 3.7;
        testpoints = FCC256();
        box = trajectory.Box(17.661,17.661,17.661);

        localq4 = shop.LocalQl(box, rcut, 4);
        localq4.computeAveNorm(testpoints);
        q4vals = localq4.getQlAveNorm();
        meanq4 = np.mean(q4vals);

        localq6 = shop.LocalQl(box, rcut, 6);
        localq6.computeAveNorm(testpoints);
        q6vals = localq6.getQlAveNorm();
        meanq6 = np.mean(q6vals);

        localq8 = shop.LocalQl(box, rcut, 8);
        localq8.computeAveNorm(testpoints);
        q8vals = localq8.getQlAveNorm();
        meanq8 = np.mean(q8vals);

        localq10 = shop.LocalQl(box, rcut, 10);
        localq10.computeAveNorm(testpoints);
        q10vals = localq10.getQlAveNorm();
        meanq10 = np.mean(q10vals);

        #Assert these tests have same value as literature
        #In Steinhardt 1983 (DOI: 10.1103/PhysRevB.28.784)  Fig2 these q are ~0.2, 0.56, 0.4, (hard to read -tiny).
        #  Note:  Not given exactly as a table, but agreement is within a few percent for l=4,6,8,10.
        npt.assert_almost_equal(meanq4, 0.19, decimal=2, err_msg="AveNormQ4fail")
        npt.assert_almost_equal(meanq6, 0.55, decimal=2, err_msg="AveNormQ6fail")
        npt.assert_almost_equal(meanq8, 0.38, decimal=2, err_msg="AveNormQ8fail")
        npt.assert_almost_equal(meanq10, 0.01, decimal=2, err_msg="AveNormQ10fail")

class TestLocalQlNear(unittest.TestCase):
    def test_QlNearfcc(self):
        rcut = 3.7;
        testpoints = FCC256();
        box = trajectory.Box(17.661,17.661,17.661);

        localq4 = shop.LocalQlNear(box, rcut, 4, 12);
        localq4.compute(testpoints);
        q4vals = localq4.getQl();
        meanq4 = np.mean(q4vals);

        localq6 = shop.LocalQlNear(box, rcut, 6, 12);
        localq6.compute(testpoints);
        q6vals = localq6.getQl();
        meanq6 = np.mean(q6vals);

        localq8 = shop.LocalQlNear(box, rcut, 8, 12);
        localq8.compute(testpoints);
        q8vals = localq8.getQl();
        meanq8 = np.mean(q8vals);

        localq10 = shop.LocalQlNear(box, rcut, 10, 12);
        localq10.compute(testpoints);
        q10vals = localq10.getQl();
        meanq10 = np.mean(q10vals);

        #Assert these tests have same value as literature
        #In Steinhardt 1983 (DOI: 10.1103/PhysRevB.28.784)  Fig2 these q are ~0.2, 0.56, 0.4, (hard to read -tiny).
        #  Note:  Not given exactly as a table, but agreement is within a few percent for l=4,6,8,10.
        npt.assert_almost_equal(meanq4, 0.19, decimal=2, err_msg="Q4Nearfail")
        npt.assert_almost_equal(meanq6, 0.56, decimal=2, err_msg="Q6Nearfail")
        npt.assert_almost_equal(meanq8, 0.38, decimal=2, err_msg="Q8Nearfail")
        npt.assert_almost_equal(meanq10, 0.09, decimal=2, err_msg="Q10Nearfail")

class TestLocalAveQlNear(unittest.TestCase):
    def test_AveQlNearfcc(self):
        rcut = 3.7;
        testpoints = FCC256();
        box = trajectory.Box(17.661,17.661,17.661);

        localq4 = shop.LocalQlNear(box, rcut, 4, 12);
        localq4.computeAve(testpoints);
        q4vals = localq4.getAveQl();
        meanq4 = np.mean(q4vals);

        localq6 = shop.LocalQlNear(box, rcut, 6, 12);
        localq6.computeAve(testpoints);
        q6vals = localq6.getAveQl();
        meanq6 = np.mean(q6vals);

        localq8 = shop.LocalQlNear(box, rcut, 8, 12);
        localq8.computeAve(testpoints);
        q8vals = localq8.getAveQl();
        meanq8 = np.mean(q8vals);

        localq10 = shop.LocalQlNear(box, rcut, 10, 12);
        localq10.computeAve(testpoints);
        q10vals = localq10.getAveQl();
        meanq10 = np.mean(q10vals);

        #Assert these tests have same value as literature
        #In Steinhardt 1983 (DOI: 10.1103/PhysRevB.28.784)  Fig2 these q are ~0.2, 0.56, 0.4, (hard to read -tiny).
        #  Note:  Not given exactly as a table, but agreement is within a few percent for l=4,6,8,10.
        npt.assert_almost_equal(meanq4, 0.19, decimal=2, err_msg="AveQ4Nearfail")
        npt.assert_almost_equal(meanq6, 0.55, decimal=2, err_msg="AveQ6Nearfail")
        npt.assert_almost_equal(meanq8, 0.38, decimal=2, err_msg="AveQ8Nearfail")
        npt.assert_almost_equal(meanq10, 0.02, decimal=2, err_msg="AveQ10Nearfail")

class TestLocalQlNormNear(unittest.TestCase):
    def test_QlNormNearfcc(self):
        rcut = 3.7;
        testpoints = FCC256();
        box = trajectory.Box(17.661,17.661,17.661);

        localq4 = shop.LocalQlNear(box, rcut, 4, 12);
        localq4.computeNorm(testpoints);
        q4vals = localq4.getQlNorm();
        meanq4 = np.mean(q4vals);

        localq6 = shop.LocalQlNear(box, rcut, 6, 12);
        localq6.computeNorm(testpoints);
        q6vals = localq6.getQlNorm();
        meanq6 = np.mean(q6vals);

        localq8 = shop.LocalQlNear(box, rcut, 8, 12);
        localq8.computeNorm(testpoints);
        q8vals = localq8.getQlNorm();
        meanq8 = np.mean(q8vals);

        localq10 = shop.LocalQlNear(box, rcut, 10, 12);
        localq10.computeNorm(testpoints);
        q10vals = localq10.getQlNorm();
        meanq10 = np.mean(q10vals);

        #Assert these tests have same value as literature
        #In Steinhardt 1983 (DOI: 10.1103/PhysRevB.28.784)  Fig2 these q are ~0.2, 0.56, 0.4, (hard to read -tiny).
        #  Note:  Not given exactly as a table, but agreement is within a few percent for l=4,6,8,10.
        npt.assert_almost_equal(meanq4, 0.19, decimal=2, err_msg="Q4NormNearfail")
        npt.assert_almost_equal(meanq6, 0.55, decimal=2, err_msg="Q6NormNearfail")
        npt.assert_almost_equal(meanq8, 0.38, decimal=2, err_msg="Q8NormNearfail")
        npt.assert_almost_equal(meanq10, 0.01, decimal=2, err_msg="Q10NormNearfail")

class TestLocalAveNormQlNear(unittest.TestCase):
    def test_AveNormQlNearfcc(self):
        rcut = 3.7;
        testpoints = FCC256();
        box = trajectory.Box(17.661,17.661,17.661);

        localq4 = shop.LocalQlNear(box, rcut, 4, 12);
        localq4.computeAveNorm(testpoints);
        q4vals = localq4.getQlAveNorm();
        meanq4 = np.mean(q4vals);

        localq6 = shop.LocalQlNear(box, rcut, 6, 12);
        localq6.computeAveNorm(testpoints);
        q6vals = localq6.getQlAveNorm();
        meanq6 = np.mean(q6vals);

        localq8 = shop.LocalQlNear(box, rcut, 8, 12);
        localq8.computeAveNorm(testpoints);
        q8vals = localq8.getQlAveNorm();
        meanq8 = np.mean(q8vals);

        localq10 = shop.LocalQlNear(box, rcut, 10, 12);
        localq10.computeAveNorm(testpoints);
        q10vals = localq10.getQlAveNorm();
        meanq10 = np.mean(q10vals);

        #Assert these tests have same value as literature
        #In Steinhardt 1983 (DOI: 10.1103/PhysRevB.28.784)  Fig2 these q are ~0.2, 0.56, 0.4, (hard to read -tiny).
        #  Note:  Not given exactly as a table, but agreement is within a few percent for l=4,6,8,10.
        npt.assert_almost_equal(meanq4, 0.19, decimal=2, err_msg="AveNormQ4Nearfail")
        npt.assert_almost_equal(meanq6, 0.55, decimal=2, err_msg="AveNormQ6Nearfail")
        npt.assert_almost_equal(meanq8, 0.38, decimal=2, err_msg="AveNormQ8Nearfail")
        npt.assert_almost_equal(meanq10, 0.01, decimal=2, err_msg="AveNormQ10Nearfail")

if __name__ == '__main__':
    unittest.main()

