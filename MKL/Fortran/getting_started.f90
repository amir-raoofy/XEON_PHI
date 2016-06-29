!*******************************************************************************
!  Content:
!    Simple Fortran test 
!
! LRZ:           Wed. Feb. 03 2016
!
!******************************************************************************/


program getting_started
  implicit none
  integer :: arows, acols, bcols, nrept, isize
  integer :: diff_count
  real(8) :: tolerance, diff_abs, correct
  character(len=1) :: prec_type
  integer :: istatus, iargcount, length
  character (len=20) :: argument
  real(4), allocatable, dimension(:) :: fa, fb
  real(8), allocatable, dimension(:) :: da, db
  real(4), allocatable, dimension(:) :: fresults, fexpect
  real(8), allocatable, dimension(:) :: dresults, dexpect

  iargcount = command_argument_count()

  arows = 512
  if( iargcount >0) then
    call get_command_argument(1, argument, length, istatus)
    call checkstatus(istatus)
    read (argument, "(I)"), arows
  endif

  prec_type ="d"
  if( iargcount >1) then
    call get_command_argument(2, argument, length, istatus)
    call checkstatus(istatus)
    read (argument, "(A1)"), prec_type
  endif

  acols = arows
  if( iargcount >2) then
    call get_command_argument(3, argument, length, istatus)
    call checkstatus(istatus)
    read (argument, "(I)"), acols
  endif

  bcols = arows
  if( iargcount >3) then
    call get_command_argument(4, argument, length, istatus)
    call checkstatus(istatus)
    read (argument, "(I)"), bcols
  endif

  nrept = 5
  if( iargcount >4) then
    call get_command_argument(5, argument, length, istatus)
    call checkstatus(istatus)
    read (argument, "(I)"), nrept
  endif

  isize = arows * bcols
  diff_count = 0
  tolerance = 1E-03
  diff_abs = tolerance

  if ('f' == prec_type) then ! single-precision
      allocate(fa(0:arows*acols-1), fb(0:acols*bcols-1),stat=istatus)
      call srandnum(fa,size(fa))
      call srandnum(fb,size(fb))
      allocate(fresults(isize), fexpect(isize),stat=istatus)
      call srun(fresults, fexpect, fa, fb, arows, acols, bcols, nrept)
      call scompare(fresults, fexpect, isize, diff_count, diff_abs)
  else if ('d' == prec_type) then ! double-precision
      allocate(da(0:arows*acols-1), db(0:acols*bcols-1),stat=istatus)
      call drandnum(da,size(da))
      call drandnum(db,size(db))
      allocate(dresults(isize), dexpect(isize),stat=istatus)
      call drun(dresults, dexpect, da, db, arows, acols, bcols, nrept)
      call dcompare(dresults, dexpect, isize, diff_count, diff_abs)
  else 
      print *, "unknown type requested!"
      stop
  endif
  correct = (isize - diff_count) / isize
  print *, "ok=",int(100.0 * correct + 0.5),"%, tol=", tolerance 
  if (0 < diff_count) print *,"err=", diff_abs;
end program getting_started


subroutine sgemm_fortran(results, a, b, arows, acols, bcols)
  implicit none
  integer :: arows, acols, bcols, brows
  real(kind=4), intent(out) :: results(0:arows*bcols-1)
  real(kind=4), intent(in)  :: a(0:arows*acols-1)
  real(kind=4), intent(in)  :: b(0:acols*bcols-1)
  integer :: j, k, r0, a0, b0, w

  brows = acols
  w = arows-1
  !$omp parallel do private(j,k,r0,a0,b0)
  do j = 0, bcols-1
    r0 = j*arows
    b0 = j*brows
    results(r0:r0+w) = 0
    do k = 0, acols-1
      a0 = k*arows
      results(r0:r0+w) = results(r0:r0+w) + a(a0:a0+w) * b(b0+k)
    enddo
  enddo
end subroutine sgemm_fortran

subroutine dgemm_fortran(results, a, b, arows, acols, bcols)
  implicit none
  integer :: arows, acols, bcols, brows
  real(kind=8), intent(out) :: results(0:arows*bcols-1)
  real(kind=8), intent(in)  :: a(0:arows*acols-1)
  real(kind=8), intent(in)  :: b(0:acols*bcols-1)
  integer :: j, k, r0, a0, b0, w

  brows = acols
  w = arows-1
  !$omp parallel do private(j,k,r0,a0,b0)
  do j = 0, bcols-1
    r0 = j*arows
    b0 = j*brows
    results(r0:r0+w) = 0
    do k = 0, acols-1
      a0 = k*arows
      results(r0:r0+w) = results(r0:r0+w) + a(a0:a0+w) * b(b0+k)
    enddo
  enddo
end subroutine dgemm_fortran

subroutine sgemm_mkl(r, a, b, arows, acols, bcols)
  implicit none
  integer, intent(in) :: arows, acols, bcols
  real(kind=4), intent(out)   :: r(0:arows*bcols-1)
  real(kind=4), intent(in)    :: a(0:arows*acols-1)
  real(kind=4), intent(in)    :: b(0:acols*bcols-1)
  real(kind=4) :: alpha, beta
  ALPHA = 1.0 
  BETA = 0.0
  call sgemm('N','N',arows,bcols,acols,ALPHA,A,arows,B,acols,BETA,r,arows)
end subroutine sgemm_mkl

subroutine dgemm_mkl(r, a, b, arows, acols, bcols)
  implicit none
  integer, intent(in) :: arows, acols, bcols
  real(kind=8), intent(out)   :: r(0:arows*bcols-1)
  real(kind=8), intent(in)    :: a(0:arows*acols-1)
  real(kind=8), intent(in)    :: b(0:acols*bcols-1)
  real(kind=8) :: alpha, beta
  ALPHA = 1.0 
  BETA = 0.0
  call dgemm('N','N',arows,bcols,acols,ALPHA,A,arows,B,acols,BETA,r,arows)
end subroutine dgemm_mkl

subroutine srun(results, expect, a, b, arows, acols, bcols, nrept)
  implicit none
  integer, intent(in) :: arows, acols, bcols, nrept
  real(kind=4), intent(out) :: results(0:arows*bcols-1)
  real(kind=4), intent(out) :: expect(0:arows*bcols-1)
  real(kind=4), intent(in)  :: a(0:arows*acols-1)
  real(kind=4), intent(in)  :: b(0:acols*bcols-1)
  integer :: i
  real(kind=8) start_time,dsecnd
  real(kind=8) :: time1,time2

  start_time =dsecnd()
  call sgemm_fortran(results, a, b, arows, acols, bcols) ! warmup
  start_time =dsecnd()
  do i = 1, nrept
    call sgemm_fortran(results, a, b, arows, acols, bcols)
  enddo
  time1 = (dsecnd() - start_time) / nrept

  call sgemm_mkl(expect, a, b, arows, acols, bcols) ! warmup
  start_time =dsecnd()
  do i = 1, nrept
    call sgemm_mkl(expect, a, b, arows, acols, bcols)
  enddo
  time2 = (dsecnd() - start_time) / nrept

  print *, "MKL: ", int(1E3 * time2 + 0.5), " ms"
  if (0 < time2) then
    print *,"   (", int(1E1 * time1 / time2 + 0.5)/1E1, "x)"
  endif
  print *, "OWN: ",int(1E3 * time1 + 0.5)," ms"
end subroutine srun

subroutine drun(results, expect, a, b, arows, acols, bcols, nrept)
  implicit none
  integer :: arows, acols, bcols, nrept
  real(kind=8), intent(out) :: results(0:arows*bcols-1)
  real(kind=8), intent(out) :: expect(0:arows*bcols-1)
  real(kind=8), intent(in)  :: a(0:arows*acols-1)
  real(kind=8), intent(in)  :: b(0:acols*bcols-1)
  integer :: i
  real(kind=8) :: start_time, dsecnd
  real(kind=4) :: time1,time2


  start_time = dsecnd()
  call dgemm_fortran(results, a, b, arows, acols, bcols) ! warmup
  start_time = dsecnd()
  do i = 1, nrept
    call dgemm_fortran(results, a, b, arows, acols, bcols)
  enddo
  time1 = (dsecnd() - start_time) / nrept

  call dgemm_mkl(expect, a, b, arows, acols, bcols) ! warmup
  start_time = dsecnd()
  do i = 1, nrept
    call dgemm_mkl(expect, a, b, arows, acols, bcols)
  enddo
  time2 = (dsecnd() - start_time) / nrept

  print *, "MKL: ", int(1E3 * time2 + 0.5), " ms"
  if (0 < time2) then
    print *,"   (", int(1E1 * time1 / time2 + 0.5)/1E1, "x)"
  endif
  print *, "OWN: ",int(1E3 * time1 + 0.5)," ms"
end subroutine drun

subroutine srandnum(a,n)
  integer n
  real, dimension(n) :: a
  real, dimension(n) :: dummy
  call random_number(dummy)
  a = 2.0 * dummy -1.0
end subroutine srandnum

subroutine drandnum(a,n)
  integer n
  real(8), dimension(n) :: a
  real, dimension(n) :: dummy
  call random_number(dummy)
  a = 2.0_8 * dummy -1.0
end subroutine drandnum

subroutine scompare(results, expect, isize, diff_count, diff_abs)
  integer :: isize
  real(4), dimension(0:isize-1) :: results, expect 
  integer :: diff_count
  real(8) :: diff_abs
  real(8) :: tolerance, d
  tolerance = abs(diff_abs)
  diff_count = 0
  diff_abs = 0
  do i = 0, isize-1
    d = abs(dble(results(i)) - dble(expect(i)))
    if (tolerance < d) then
      diff_abs = max(diff_abs, d)
      diff_count = diff_count + 1
    endif
  enddo
end subroutine scompare

subroutine dcompare(results, expect, isize, diff_count, diff_abs)
  integer :: isize
  real(8), dimension(0:isize-1) :: results, expect 
  integer :: diff_count
  real(8) :: diff_abs
  real(8) :: tolerance, d
  tolerance = abs(diff_abs)
  diff_count = 0
  diff_abs = 0
  do i = 0, isize-1
    d = abs(results(i) - expect(i))
    if (tolerance < d) then
      diff_abs = max(diff_abs, d)
      diff_count = diff_count + 1
    endif
  enddo
end subroutine dcompare

subroutine checkstatus(istatus)
  integer istatus
  if(istatus /= 0) then
    print *, "Error reading command line or allocating arrays"
    stop
  end if
end subroutine checkstatus

