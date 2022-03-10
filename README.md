# ldmx-framework-bench

<a href="https://github.com/tomeichlersmith/ldmx-rootless/actions" alt="Actions">
    <img src="https://github.com/tomeichlersmith/ldmx-rootless/workflows/CI/badge.svg" />
</a>

Trying to drag ldmx-sw into the 21st century by removing our dependency on the archaic ROOT.
This repository is specifically geared towards benchmarks comparing the ROOT-based and HDF5-based frameworks.

## Q&A
- Why?
  - ROOT fails to conform to modern software practices causing it to not be used outside of HEP.
    This means it is difficult to learn for new users, is poorly maintained, and has poor documentation.
  - Since ROOT's inception and standardization at CERN, modern industry big data tools have
    come into existence that are as performative, easier to use, and better documented than ROOT.
  - ROOT's lack of clear exception and error handling leads to segmentation violations instead of error codes and error messages. This leads to development and maintenance of software to be incredibly arduous.
- ROOT is highly performative for our usecase, will we see reduced speed?
  - I have been developing a [benchmark test](Bench) comparing ROOT and HDF5 frameworks.
  - This test actually shows that the HDF5-based framework is five times faster than the ROOT framework.
- ROOT has long term-support, why not rely on that?
  - HDF5, matplotlib, pandas, and other industry tools are well supported by the wider (non-HEP) community,
    so we can safely rely on them to be maintained in the future. Moreover, since they have a wider user base,
    their development has surpassed ROOT in terms of stability and ease-of-use.
- ROOT allows for quick data analysis, why abandon it?
  - I disagree that ROOT allows for quick data analysis, especially for new users.
    ROOT is very opaque and difficult to understand, and I think you are underestimating how quick analysis can be with other tools.
  - One standard analysis technique is using `matplotlib` inside a Jupyter notebook.
    That is incredibly quick, flexible, and powerful.
- Translation layer for intermediate stage?
  - Transition to HDF5 will not happen until reading files produced by ROOT-based framework is enabled in the new framework [fire#19](https://github.com/LDMX-Software/fire/issues/19)
- Is ROOT broken?
  - Yes. It is not a coincidence that LDMX (and many other HEP experiments) have struggled to get basic abilities working within their processing frameworks. For example, [it took LDMX many years to allow multiple input files to be read and copied into one output file](https://github.com/LDMX-Software/ldmx-sw/issues/253). It is no great talent of mine that allowed the HDF5-based framework to achieve the same abilities in the matter of weeks - HDF5 and its downstream libraries are much easier to use in development. The other indirect evidence that ROOT is broken is that no one outside of HEP (the people who already use it) choose to use it.
- Do we really have the extra personnel for the effort to make the transition if it isn't?
  - I don't believe we have extra personnel to make this transition and therefore I am not pushing for it to be rushed. This will be a side project for me (and others who are interested) which will be explicitly the lowest priority. Like many others in the collaboration, I have other responsibilities and I put all of those other responsibilities above working on this transition. We do not plan to even begin a collaboration-wide transition until the HDF5-based framework and ldmx-sw itself have been developed to a comfortable position (as assessed by those attending the software developers meeting and anyone else who wishes to reach out to me and be informed).
- Who will understand if you two move on to other positions?
  - I have three answers to this question.
  - First (and most directly), I have had occasional meetings with Jeremy about the inner workings of the HDF5-based framework. He helps initially develop the ROOT-based framework and so would be a natural candidate to the the "permanent" position who can help future developers understand the framework.
  - Second, I have put a large effort into developing [the documentation of the HDF5-based framework](https://ldmx-software.github.io/fire/). It is already more well-documented than our current framework (in my opinion).
  - Finally, I have a very sassy answer (so please don't take it too seriously). My knee-jerk response to this question is Who understands the current framework now? I don't think it is unrealistic to say that I can count on one hand the people I would label as "understanding" of the current framework (and only one of them is in a permanent position).
- What about [this HEP study](https://iopscience.iop.org/article/10.1088/1742-6596/1085/3/032020/pdf) claiming ROOT outperforms HDF5?
  - The biggest issue with this study is that it accesses all other data storage libraries in the same pattern that it access's ROOT. This is a wildly inaccurate comparison because ROOT does internal caching while most (if not all) of the other libraries mentioned do not do their own caching. I said this in the last general meeting but you may have missed it: When you tell ROOT to write something, it just puts it in the cache while when you tell HDF5 to write something, it writes to disk. After implementing a caching procedure, HDF5 is five times faster than ROOT across 6 orders of magnitude in the number of events (as shown in the [Bench](Bench) directory of this repo).
- What about [Moving away from HDF5](https://cyrille.rossant.net/moving-away-hdf5/) (a blog post explaining why a neuroscience python library chose to stop using HDF5)?
  - The critiques presented in this blog post can either be applied to ROOT, have been fixed since the posting in 2016, or both. Examples:
    - Lack of opacity: whenever dealing with low-level, high-performance file writing, the code will not be "easy" to understand. HDF5's wide use actually allows its downstream libraries like HighFive and h5py to be easier to use.
    - Corruption risks: Patched in HDF5 v1.12 (the latest version that we intend to use)
    - Lack of good parallel access: ROOT lacks this and HDF5 and its python package h5py have improved greatly upon it
    - Various limitations and bugs: _caution, sarcasm ahead_ If the phrase _various limitations and bugs_ doesn't describe the ROOT library, I don't know what does. Of course both ROOT and HDF5 will have limitations and bugs. The fact that HDF5 is more widely used means that avoiding the limitations and getting the bugs patches will be easier.
  - This post also points out that the users of their library will still need to decide on a file format, which makes sense for them. This library is designed to be a set of common tools for similar experiments and therefore they are choosing to avoid picking a file format to support.
- Why not just use [uproot](https://github.com/scikit-hep/uproot4) to allow more modern access to data in ROOT files?
  - Many in HEP (and our collaboration) use uproot to read in our data files and do their analyses. Personally, I also use uproot occasionally and think it is a good package and good solution when the data to be analyzed is already in ROOT files.
  - The other reasons for transitioning to HDF5 besides more moden access to data is what comes into play in this question. Specifically, the easier development/maintenance of the framework (in my opinion) and the higher performance of the updated framework (quantitative: results in [Bench](Bench)) are good reasons _alone_ to move to an HDF5-based framework and data format compared to ROOT.

## Environment Notes

A container image with HDF5, Boost, and HighFive installed has been built in the LDMX-Software/docker
repository on the hdf5 branch. I have copied over the `ldmx-env.sh` script here and modified it slightly
as well as copied over the `.ldmxrc` file and updated it to have the current directory be the base directory
and the default container image be the hdf5 image.

## Future Plans
- `fire` versioned and installed in developers container
- Jupyter notebook installed in dev container and port opened through env script
