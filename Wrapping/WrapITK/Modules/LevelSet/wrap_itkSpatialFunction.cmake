WRAP_CLASS("itk::SpatialFunction")
  foreach(d ${WRAP_ITK_DIMS})
    WRAP_TEMPLATE("${ITKM_D}${d}${ITKM_PD${d}}" "${ITKT_D},${d},${ITKT_PD${d}}")
  endforeach(d)
END_WRAP_CLASS()
