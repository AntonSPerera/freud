import numpy as np
import freud
import unittest
import util


class TestInterface(unittest.TestCase):
    def test_take_one(self):
        """Test that there is exactly 1 or 12 particles at the interface when
        one particle is removed from an FCC structure"""
        np.random.seed(0)
        (box, positions) = util.make_fcc(4, 4, 4, noise=1e-2)
        positions.flags['WRITEABLE'] = False

        index = np.random.randint(0, len(positions))

        point = positions[index].reshape((1, 3))
        others = np.concatenate([positions[:index], positions[index + 1:]])

        inter = freud.interface.InterfaceMeasure(1.5)

        # Test attribute access
        with self.assertRaises(AttributeError):
            inter.point_count
        with self.assertRaises(AttributeError):
            inter.point_ids
        with self.assertRaises(AttributeError):
            inter.query_point_count
        with self.assertRaises(AttributeError):
            inter.query_point_ids

        test_one = inter.compute(box, point, others)

        # Test attribute access
        inter.point_count
        inter.point_ids
        inter.query_point_count
        inter.query_point_ids

        self.assertEqual(test_one.point_count, 1)
        self.assertEqual(len(test_one.point_ids), 1)

        test_twelve = inter.compute(box, others, point)
        self.assertEqual(test_twelve.point_count, 12)
        self.assertEqual(len(test_twelve.point_ids), 12)

    def test_filter_r(self):
        """Test that nlists are filtered to the correct r_max."""
        np.random.seed(0)
        r_max = 3.0
        (box, positions) = util.make_fcc(4, 4, 4, noise=1e-2)

        index = np.random.randint(0, len(positions))

        point = positions[index].reshape((1, 3))
        others = np.concatenate([positions[:index], positions[index + 1:]])

        # Creates a neighborlist with r_max larger than the interface r_max
        lc = freud.locality.LinkCell(box, r_max, others)
        nlist = lc.query(point, dict(r_max=r_max)).toNeighborList()

        inter = freud.interface.InterfaceMeasure(1.5)

        test_twelve = inter.compute(box, others, point, nlist)
        self.assertEqual(test_twelve.point_count, 12)
        self.assertEqual(len(test_twelve.point_ids), 12)

    def test_repr(self):
        inter = freud.interface.InterfaceMeasure(1.5)
        self.assertEqual(str(inter), str(eval(repr(inter))))


if __name__ == '__main__':
    unittest.main()
