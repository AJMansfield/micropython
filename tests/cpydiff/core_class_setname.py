"""
categories: Core,Classes
description: The order and selection for which class members __set_name__ is called on is inconsistent when the parent class is mutated within __set_name__.
cause: MicroPython dictionary iteration order does not match CPython order.
workaround: Don't add or remove class members that define __set_name__ in __set_name__ implementations.
"""


## Test harness for class-creation-time dict-order feng-shui
def chain(*args):
    """Equivalent to ``itertools.chain``."""
    for arg in args:
        yield from arg

class Del:
    def __repr__(self):
        return "DEL"
DEL = Del()
"""Deletion sentinel for descriptor modification execution."""

class Descriptor:
    """Convenience descriptor for setting up class modifications to happen at __set_name__ time."""
    def __init__(self, *args, **kw):
        self.args = args
        self.ops = [] # type: list[tuple[str, ...]]
        self.op(**kw)
    
    def op(self, **kw):
        """Factory method for working around disordered kwargs and adding additional ops on same name."""
        self.ops.extend([(k,v) for k,v in kw.items()])
        return self

    def __set_name__(self, owner, name):
        print(f"  {self!r}.__set_name__({owner.__name__}, {name!r})")
        for name, value in self.ops:
            if value is DEL:
                try:
                    delattr(owner, name)
                    print("    del", name)
                except AttributeError:
                    print("    nop", name)
            else:
                print("    set", name, value)
                setattr(owner, name, value)
    
    def __repr__(self):
        argit = (f"{v!r}" for v in self.args)
        kwargit = (f"{k}={v!r}" for k,v in self.ops)
        paramstr = ", ".join(chain(argit, kwargit))
        return f"Descriptor({paramstr})"

class DescriptorHost:
    """Convenience parent to inherit a useful repr from."""
    def __repr__(self):
        elemstr = ", ".join(f"{k}={v!r}" for k,v in self.__class__.__dict__.items() if not k.startswith('_'))
        return f"{self.__class__.__name__}({elemstr})"



## Test Cases

class MutualDelete(DescriptorHost):
    _aa=_ab=_ac=_ad=_ae=_af=_ag=_ah=_ai=_aj=_ak=_al=_am=_an=_ao=_ap=_aq=_ar=_as=_at=_au=_av=_aw=_ax=_ay=_az=_ba=_bb=_bc=_bd=_be=_bf=_bg=_bh=_bi=_bj=_bk=_bl=_bm=_bn=_bo=_bp=_bq=_br=_bs=_bt=_bu=_bv=_bw=_bx=_by=_bz=_ca=_cb=_cc=_cd=_ce=_cf=_cg=_ch=_ci=_cj=_ck=_cl=_cm=_cn=_co=_cp=_cq=_cr=_cs=_ct=_cu=_cv=_cw=_cx=_cy=_cz=_da=_db=_dc=_dd=_de=_df=_dg=_dh=_di=_dj=_dk=_dl=_dm=_dn=_do=_dp=_dq=_dr=_ds=_dt=_du=_dv=_dw=_dx=_dy=_dz=_ea=_eb=_ec=_ed=_ee=_ef=_eg=_eh=_ei=_ej=_ek=_el=_em=_en=_eo=_ep=_eq=_er=_es=_et=_eu=_ev=_ew=_ex=_ey=_ez=_fa=_fb=_fc=_fd=_fe=_ff=_fg=_fh=_fi=_fj=_fk=_fl=_fm=_fn=_fo=_fp=_fq=_fr=_fs=_ft=_fu=_fv=_fw=_fx=_fy=_fz=_ga=_gb=_gc=_gd=_ge=_gf=_gg=_gh=_gi=_gj=_gk=_gl=_gm=_gn=_go=_gp=_gq=_gr=_gs=_gt=_gu=_gv=_gw=_gx=_gy=_gz=_ha=_hb=_hc=_hd=_he=_hf=_hg=_hh=_hi=_hj=_hk=_hl=_hm=_hn=_ho=_hp=_hq=_hr=_hs=_ht=_hu=_hv=_hw=_hx=_hy=_hz=_ia=_ib=_ic=_id=_ie=_if=_ig=_ih=_ii=_ij=_ik=_il=_im=_in=_io=_ip=_iq=_ir=_is=_it=_iu=_iv=_iw=_ix=_iy=_iz=_ja=_jb=_jc=_jd=_je=_jf=_jg=_jh=_ji=_jj=_jk=_jl=_jm=_jn=_jo=_jp=_jq=_jr=_js=_jt=_ju=_jv=_jw=_jx=_jy=_jz=_ka=_kb=_kc=_kd=_ke=_kf=_kg=_kh=_ki=_kj=_kk=_kl=_km=_kn=_ko=_kp=_kq=_kr=_ks=_kt=_ku=_kv=_kw=_kx=_ky=_kz=_la=_lb=_lc=_ld=_le=_lf=_lg=_lh=_li=_lj=_lk=_ll=_lm=_ln=_lo=_lp=_lq=_lr=_ls=_lt=_lu=_lv=_lw=_lx=_ly=_lz=_ma=_mb=_mc=_md=_me=_mf=_mg=_mh=_mi=_mj=_mk=_ml=_mm=_mn=_mo=_mp=_mq=_mr=_ms=_mt=_mu=_mv=_mw=_mx=_my=_mz=_na=_nb=_nc=_nd=_ne=_nf=_ng=_nh=_ni=_nj=_nk=_nl=_nm=_nn=_no=_np=_nq=_nr=_ns=_nt=_nu=_nv=_nw=_nx=_ny=_nz=_oa=_ob=_oc=_od=_oe=_of=_og=_oh=_oi=_oj=_ok=_ol=_om=_on=_oo=_op=_oq=_or=_os=_ot=_ou=_ov=_ow=_ox=_oy=_oz=_pa=_pb=_pc=_pd=_pe=_pf=_pg=_ph=_pi=_pj=_pk=_pl=_pm=_pn=_po=_pp=_pq=_pr=_ps=_pt=_pu=_pv=_pw=_px=_py=_pz=_qa=_qb=_qc=_qd=_qe=_qf=_qg=_qh=_qi=_qj=_qk=_ql=_qm=_qn=_qo=_qp=_qq=_qr=_qs=_qt=_qu=_qv=_qw=_qx=_qy=_qz=_ra=_rb=_rc=_rd=_re=_rf=_rg=_rh=_ri=_rj=_rk=_rl=_rm=_rn=_ro=_rp=_rq=_rr=_rs=_rt=_ru=_rv=_rw=_rx=_ry=_rz=_sa=_sb=_sc=_sd=_se=_sf=_sg=_sh=_si=_sj=_sk=_sl=_sm=_sn=_so=_sp=_sq=_sr=_ss=_st=_su=_sv=_sw=_sx=_sy=_sz=_ta=_tb=_tc=_td=_te=_tf=_tg=_th=_ti=_tj=_tk=_tl=_tm=_tn=_to=_tp=_tq=_tr=_ts=_tt=_tu=_tv=_tw=_tx=_ty=_tz=_ua=_ub=_uc=_ud=_ue=_uf=_ug=_uh=_ui=_uj=_uk=_ul=_um=_un=_uo=_up=_uq=_ur=_us=_ut=_uu=_uv=_uw=_ux=_uy=_uz=_va=_vb=_vc=_vd=_ve=_vf=_vg=_vh=_vi=_vj=_vk=_vl=_vm=_vn=_vo=_vp=_vq=_vr=_vs=_vt=_vu=_vv=_vw=_vx=_vy=_vz=_wa=_wb=_wc=_wd=_we=_wf=_wg=_wh=_wi=_wj=_wk=_wl=_wm=_wn=_wo=_wp=_wq=_wr=_ws=_wt=_wu=_wv=_ww=_wx=_wy=_wz=_xa=_xb=_xc=_xd=_xe=_xf=_xg=_xh=_xi=_xj=_xk=_xl=_xm=_xn=_xo=_xp=_xq=_xr=_xs=_xt=_xu=_xv=_xw=_xx=_xy=_xz=_ya=_yb=_yc=_yd=_ye=_yf=_yg=_yh=_yi=_yj=_yk=_yl=_ym=_yn=_yo=_yp=_yq=_yr=_ys=_yt=_yu=_yv=_yw=_yx=_yy=_yz=_za=_zb=_zc=_zd=_ze=_zf=_zg=_zh=_zi=_zj=_zk=_zl=_zm=_zn=_zo=_zp=_zq=_zr=_zs=_zt=_zu=_zv=_zw=_zx=_zy=_zz=0
    a = Descriptor("a").op(b=DEL)
    b = Descriptor("b").op(a=DEL)
    print("class MutualDelete")
print(MutualDelete())



class CreateEarlierSibling(DescriptorHost):
    _aa=_ab=_ac=_ad=_ae=_af=_ag=_ah=_ai=_aj=_ak=_al=_am=_an=_ao=_ap=_aq=_ar=_as=_at=_au=_av=_aw=_ax=_ay=_az=_ba=_bb=_bc=_bd=_be=_bf=_bg=_bh=_bi=_bj=_bk=_bl=_bm=_bn=_bo=_bp=_bq=_br=_bs=_bt=_bu=_bv=_bw=_bx=_by=_bz=_ca=_cb=_cc=_cd=_ce=_cf=_cg=_ch=_ci=_cj=_ck=_cl=_cm=_cn=_co=_cp=_cq=_cr=_cs=_ct=_cu=_cv=_cw=_cx=_cy=_cz=_da=_db=_dc=_dd=_de=_df=_dg=_dh=_di=_dj=_dk=_dl=_dm=_dn=_do=_dp=_dq=_dr=_ds=_dt=_du=_dv=_dw=_dx=_dy=_dz=_ea=_eb=_ec=_ed=_ee=_ef=_eg=_eh=_ei=_ej=_ek=_el=_em=_en=_eo=_ep=_eq=_er=_es=_et=_eu=_ev=_ew=_ex=_ey=_ez=_fa=_fb=_fc=_fd=_fe=_ff=_fg=_fh=_fi=_fj=_fk=_fl=_fm=_fn=_fo=_fp=_fq=_fr=_fs=_ft=_fu=_fv=_fw=_fx=_fy=_fz=_ga=_gb=_gc=_gd=_ge=_gf=_gg=_gh=_gi=_gj=_gk=_gl=_gm=_gn=_go=_gp=_gq=_gr=_gs=_gt=_gu=_gv=_gw=_gx=_gy=_gz=_ha=_hb=_hc=_hd=_he=_hf=_hg=_hh=_hi=_hj=_hk=_hl=_hm=_hn=_ho=_hp=_hq=_hr=_hs=_ht=_hu=_hv=_hw=_hx=_hy=_hz=_ia=_ib=_ic=_id=_ie=_if=_ig=_ih=_ii=_ij=_ik=_il=_im=_in=_io=_ip=_iq=_ir=_is=_it=_iu=_iv=_iw=_ix=_iy=_iz=_ja=_jb=_jc=_jd=_je=_jf=_jg=_jh=_ji=_jj=_jk=_jl=_jm=_jn=_jo=_jp=_jq=_jr=_js=_jt=_ju=_jv=_jw=_jx=_jy=_jz=_ka=_kb=_kc=_kd=_ke=_kf=_kg=_kh=_ki=_kj=_kk=_kl=_km=_kn=_ko=_kp=_kq=_kr=_ks=_kt=_ku=_kv=_kw=_kx=_ky=_kz=_la=_lb=_lc=_ld=_le=_lf=_lg=_lh=_li=_lj=_lk=_ll=_lm=_ln=_lo=_lp=_lq=_lr=_ls=_lt=_lu=_lv=_lw=_lx=_ly=_lz=_ma=_mb=_mc=_md=_me=_mf=_mg=_mh=_mi=_mj=_mk=_ml=_mm=_mn=_mo=_mp=_mq=_mr=_ms=_mt=_mu=_mv=_mw=_mx=_my=_mz=_na=_nb=_nc=_nd=_ne=_nf=_ng=_nh=_ni=_nj=_nk=_nl=_nm=_nn=_no=_np=_nq=_nr=_ns=_nt=_nu=_nv=_nw=_nx=_ny=_nz=_oa=_ob=_oc=_od=_oe=_of=_og=_oh=_oi=_oj=_ok=_ol=_om=_on=_oo=_op=_oq=_or=_os=_ot=_ou=_ov=_ow=_ox=_oy=_oz=_pa=_pb=_pc=_pd=_pe=_pf=_pg=_ph=_pi=_pj=_pk=_pl=_pm=_pn=_po=_pp=_pq=_pr=_ps=_pt=_pu=_pv=_pw=_px=_py=_pz=_qa=_qb=_qc=_qd=_qe=_qf=_qg=_qh=_qi=_qj=_qk=_ql=_qm=_qn=_qo=_qp=_qq=_qr=_qs=_qt=_qu=_qv=_qw=_qx=_qy=_qz=_ra=_rb=_rc=_rd=_re=_rf=_rg=_rh=_ri=_rj=_rk=_rl=_rm=_rn=_ro=_rp=_rq=_rr=_rs=_rt=_ru=_rv=_rw=_rx=_ry=_rz=_sa=_sb=_sc=_sd=_se=_sf=_sg=_sh=_si=_sj=_sk=_sl=_sm=_sn=_so=_sp=_sq=_sr=_ss=_st=_su=_sv=_sw=_sx=_sy=_sz=_ta=_tb=_tc=_td=_te=_tf=_tg=_th=_ti=_tj=_tk=_tl=_tm=_tn=_to=_tp=_tq=_tr=_ts=_tt=_tu=_tv=_tw=_tx=_ty=_tz=_ua=_ub=_uc=_ud=_ue=_uf=_ug=_uh=_ui=_uj=_uk=_ul=_um=_un=_uo=_up=_uq=_ur=_us=_ut=_uu=_uv=_uw=_ux=_uy=_uz=_va=_vb=_vc=_vd=_ve=_vf=_vg=_vh=_vi=_vj=_vk=_vl=_vm=_vn=_vo=_vp=_vq=_vr=_vs=_vt=_vu=_vv=_vw=_vx=_vy=_vz=_wa=_wb=_wc=_wd=_we=_wf=_wg=_wh=_wi=_wj=_wk=_wl=_wm=_wn=_wo=_wp=_wq=_wr=_ws=_wt=_wu=_wv=_ww=_wx=_wy=_wz=_xa=_xb=_xc=_xd=_xe=_xf=_xg=_xh=_xi=_xj=_xk=_xl=_xm=_xn=_xo=_xp=_xq=_xr=_xs=_xt=_xu=_xv=_xw=_xx=_xy=_xz=_ya=_yb=_yc=_yd=_ye=_yf=_yg=_yh=_yi=_yj=_yk=_yl=_ym=_yn=_yo=_yp=_yq=_yr=_ys=_yt=_yu=_yv=_yw=_yx=_yy=_yz=_za=_zb=_zc=_zd=_ze=_zf=_zg=_zh=_zi=_zj=_zk=_zl=_zm=_zn=_zo=_zp=_zq=_zr=_zs=_zt=_zu=_zv=_zw=_zx=_zy=_zz=0
    # a = Descriptor("a") # to be created
    b = Descriptor("b")
    c = Descriptor("c").op(a=Descriptor("a"))
    print("class CreateEarlierSibling")
print(CreateEarlierSibling())



class CreateLaterSibling(DescriptorHost):
    _aa=_ab=_ac=_ad=_ae=_af=_ag=_ah=_ai=_aj=_ak=_al=_am=_an=_ao=_ap=_aq=_ar=_as=_at=_au=_av=_aw=_ax=_ay=_az=_ba=_bb=_bc=_bd=_be=_bf=_bg=_bh=_bi=_bj=_bk=_bl=_bm=_bn=_bo=_bp=_bq=_br=_bs=_bt=_bu=_bv=_bw=_bx=_by=_bz=_ca=_cb=_cc=_cd=_ce=_cf=_cg=_ch=_ci=_cj=_ck=_cl=_cm=_cn=_co=_cp=_cq=_cr=_cs=_ct=_cu=_cv=_cw=_cx=_cy=_cz=_da=_db=_dc=_dd=_de=_df=_dg=_dh=_di=_dj=_dk=_dl=_dm=_dn=_do=_dp=_dq=_dr=_ds=_dt=_du=_dv=_dw=_dx=_dy=_dz=_ea=_eb=_ec=_ed=_ee=_ef=_eg=_eh=_ei=_ej=_ek=_el=_em=_en=_eo=_ep=_eq=_er=_es=_et=_eu=_ev=_ew=_ex=_ey=_ez=_fa=_fb=_fc=_fd=_fe=_ff=_fg=_fh=_fi=_fj=_fk=_fl=_fm=_fn=_fo=_fp=_fq=_fr=_fs=_ft=_fu=_fv=_fw=_fx=_fy=_fz=_ga=_gb=_gc=_gd=_ge=_gf=_gg=_gh=_gi=_gj=_gk=_gl=_gm=_gn=_go=_gp=_gq=_gr=_gs=_gt=_gu=_gv=_gw=_gx=_gy=_gz=_ha=_hb=_hc=_hd=_he=_hf=_hg=_hh=_hi=_hj=_hk=_hl=_hm=_hn=_ho=_hp=_hq=_hr=_hs=_ht=_hu=_hv=_hw=_hx=_hy=_hz=_ia=_ib=_ic=_id=_ie=_if=_ig=_ih=_ii=_ij=_ik=_il=_im=_in=_io=_ip=_iq=_ir=_is=_it=_iu=_iv=_iw=_ix=_iy=_iz=_ja=_jb=_jc=_jd=_je=_jf=_jg=_jh=_ji=_jj=_jk=_jl=_jm=_jn=_jo=_jp=_jq=_jr=_js=_jt=_ju=_jv=_jw=_jx=_jy=_jz=_ka=_kb=_kc=_kd=_ke=_kf=_kg=_kh=_ki=_kj=_kk=_kl=_km=_kn=_ko=_kp=_kq=_kr=_ks=_kt=_ku=_kv=_kw=_kx=_ky=_kz=_la=_lb=_lc=_ld=_le=_lf=_lg=_lh=_li=_lj=_lk=_ll=_lm=_ln=_lo=_lp=_lq=_lr=_ls=_lt=_lu=_lv=_lw=_lx=_ly=_lz=_ma=_mb=_mc=_md=_me=_mf=_mg=_mh=_mi=_mj=_mk=_ml=_mm=_mn=_mo=_mp=_mq=_mr=_ms=_mt=_mu=_mv=_mw=_mx=_my=_mz=_na=_nb=_nc=_nd=_ne=_nf=_ng=_nh=_ni=_nj=_nk=_nl=_nm=_nn=_no=_np=_nq=_nr=_ns=_nt=_nu=_nv=_nw=_nx=_ny=_nz=_oa=_ob=_oc=_od=_oe=_of=_og=_oh=_oi=_oj=_ok=_ol=_om=_on=_oo=_op=_oq=_or=_os=_ot=_ou=_ov=_ow=_ox=_oy=_oz=_pa=_pb=_pc=_pd=_pe=_pf=_pg=_ph=_pi=_pj=_pk=_pl=_pm=_pn=_po=_pp=_pq=_pr=_ps=_pt=_pu=_pv=_pw=_px=_py=_pz=_qa=_qb=_qc=_qd=_qe=_qf=_qg=_qh=_qi=_qj=_qk=_ql=_qm=_qn=_qo=_qp=_qq=_qr=_qs=_qt=_qu=_qv=_qw=_qx=_qy=_qz=_ra=_rb=_rc=_rd=_re=_rf=_rg=_rh=_ri=_rj=_rk=_rl=_rm=_rn=_ro=_rp=_rq=_rr=_rs=_rt=_ru=_rv=_rw=_rx=_ry=_rz=_sa=_sb=_sc=_sd=_se=_sf=_sg=_sh=_si=_sj=_sk=_sl=_sm=_sn=_so=_sp=_sq=_sr=_ss=_st=_su=_sv=_sw=_sx=_sy=_sz=_ta=_tb=_tc=_td=_te=_tf=_tg=_th=_ti=_tj=_tk=_tl=_tm=_tn=_to=_tp=_tq=_tr=_ts=_tt=_tu=_tv=_tw=_tx=_ty=_tz=_ua=_ub=_uc=_ud=_ue=_uf=_ug=_uh=_ui=_uj=_uk=_ul=_um=_un=_uo=_up=_uq=_ur=_us=_ut=_uu=_uv=_uw=_ux=_uy=_uz=_va=_vb=_vc=_vd=_ve=_vf=_vg=_vh=_vi=_vj=_vk=_vl=_vm=_vn=_vo=_vp=_vq=_vr=_vs=_vt=_vu=_vv=_vw=_vx=_vy=_vz=_wa=_wb=_wc=_wd=_we=_wf=_wg=_wh=_wi=_wj=_wk=_wl=_wm=_wn=_wo=_wp=_wq=_wr=_ws=_wt=_wu=_wv=_ww=_wx=_wy=_wz=_xa=_xb=_xc=_xd=_xe=_xf=_xg=_xh=_xi=_xj=_xk=_xl=_xm=_xn=_xo=_xp=_xq=_xr=_xs=_xt=_xu=_xv=_xw=_xx=_xy=_xz=_ya=_yb=_yc=_yd=_ye=_yf=_yg=_yh=_yi=_yj=_yk=_yl=_ym=_yn=_yo=_yp=_yq=_yr=_ys=_yt=_yu=_yv=_yw=_yx=_yy=_yz=_za=_zb=_zc=_zd=_ze=_zf=_zg=_zh=_zi=_zj=_zk=_zl=_zm=_zn=_zo=_zp=_zq=_zr=_zs=_zt=_zu=_zv=_zw=_zx=_zy=_zz=0
    a = Descriptor("a").op(c=Descriptor("c"))
    b = Descriptor("b")
    # c = Descriptor("c") # to be created
    print("class CreateLaterSibling")
print(CreateLaterSibling())