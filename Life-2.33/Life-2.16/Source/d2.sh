diff -p -w -y --ignore-all-space  --suppress-common-lines built_ins.c built_ins.bad.c >diff2.out
diff -p -w -y --ignore-all-space  --suppress-common-lines parser.c parser.bad.c >>diff2.out
diff -p -w -y --ignore-all-space  --suppress-common-lines types.c types.bad.c  >>diff2.out
emacs diff2.out
