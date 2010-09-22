# auto include feature must be disable because the class is not in the file
# with the same name
set(WRAPPER_AUTO_INCLUDE_HEADERS OFF)
WRAP_INCLUDE("vnl/vnl_matrix_ref.h")

WRAP_CLASS("vnl_matrix_ref")
  WRAP_TEMPLATE("${ITKM_D}" "${ITKT_D}")
  WRAP_TEMPLATE("${ITKM_F}" "${ITKT_F}")
END_WRAP_CLASS()
