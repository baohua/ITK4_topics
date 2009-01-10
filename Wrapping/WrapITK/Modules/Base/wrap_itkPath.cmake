WRAP_CLASS("itk::Path" POINTER)
  FOREACH(d ${WRAP_ITK_DIMS})
    WRAP_TEMPLATE("${ITKM_D}${ITKM_CID${d}}${d}" "${ITKT_D}, ${ITKT_CID${d}}, ${d}")
  ENDFOREACH(d)
END_WRAP_CLASS()