__author__ = 'david'


class LabelDataset:
    @property
    def count(self):
        return self.strings.shape[0]
