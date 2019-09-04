from setuptools import Extension, setup

extension = Extension("gcparser", ["gcparser.c", ])

setup(
    name='gcparser',
    ext_modules=[extension]
)
