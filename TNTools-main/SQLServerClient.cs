using Microsoft.Data.SqlClient;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Data;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using static System.Windows.Forms.VisualStyles.VisualStyleElement.StartPanel;

namespace TNTools
{
    public static class SQLServerClient
    {
        private static string _strConnection = "Data Source=171.244.49.76,1433;Initial Catalog=U2202_Prod_TXNG_V5;User Id=admin;Password=mtevs3vpSXvaV;Pooling=false;TrustServerCertificate=True";

        public static void CreateConnection(string connString)
        {

        }

        public static string GenerateGLNAndGCPCode(string username, int GLNCount, int GCPCount)
        {
            using (SqlConnection con = new SqlConnection(_strConnection))
            {
                con.Open();
                var userId = "";
                using (SqlCommand cmd = new SqlCommand("SELECT*FROM U2202_Prod_Core_V5.Identityserver.Users WHERE UserName = @UserName", con))
                {
                    cmd.Parameters.AddWithValue("@UserName", username);
                    using (SqlDataReader reader = cmd.ExecuteReader())
                    {
                        while (reader.Read())
                        {
                            userId = reader["Id"].ToString();
                        }
                    }
                }

                if (string.IsNullOrEmpty(userId))
                    return "Không tìm thấy tài khoản";

                var toChucHoSoId = "";
                using (SqlCommand cmd = new SqlCommand("SELECT * FROM U2202_Prod_ToChuc_V5.dbo.ToChuc_HoSo WHERE UserId = @UserId", con))
                {
                    cmd.Parameters.AddWithValue("@UserId", userId);
                    using (SqlDataReader reader = cmd.ExecuteReader())
                    {
                        while (reader.Read())
                        {
                            toChucHoSoId = reader["Id"].ToString();
                        }
                    }
                }
                if (string.IsNullOrEmpty(toChucHoSoId))
                    return "Không tìm thấy thông tin hồ sơ tổ chức";

                if (GCPCount > 0)
                {
                    using (SqlCommand cmd = new SqlCommand($"UPDATE U2202_Prod_KhoMa_V5.dbo.KhoMa_GCP SET IdToChucHoSo = @IdToChucHoSo, TrangThai= 2 WHERE Id IN (SELECT top {GCPCount} s.Id FROM U2202_Prod_KhoMa_V5.dbo.KhoMa_GCP s WHERE TrangThai in (6,7))", con))
                    {
                        cmd.Parameters.AddWithValue("@IdToChucHoSo", toChucHoSoId);
                        cmd.ExecuteNonQuery();
                    }
                }
                if (GLNCount > 0)
                {
                    using (SqlCommand cmd = new SqlCommand($"UPDATE U2202_Prod_KhoMa_V5.dbo.KhoMa_GLN SET IdToChucHoSo = @IdToChucHoSo, TrangThai= 2 WHERE Id IN (SELECT top {GLNCount} s.Id FROM U2202_Prod_KhoMa_V5.dbo.KhoMa_GLN s WHERE TrangThai in (6,7))", con))
                    {
                        cmd.Parameters.AddWithValue("@IdToChucHoSo", toChucHoSoId);
                        cmd.ExecuteNonQuery();
                    }
                }
            }
            return "Thực hiện thành công";
        }
    }
}